// Vita3K emulator project
// Copyright (C) 2026 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

package io.github.vitarecomps.frontend;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.graphics.Typeface;
import android.os.SystemClock;
import android.util.SparseIntArray;
import android.view.DisplayCutout;
import android.view.HapticFeedbackConstants;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowInsets;

import org.libsdl.app.SDLActivity;

import java.util.ArrayList;
import java.util.List;

/**
 * On-screen controls over the game (ss-delta 09-android, "On-screen controls"), for phones without a
 * gamepad: the Vita's buttons, D-pad and both sticks, plus a button for the frontend's settings menu.
 * What they hold goes to the game through game-frontend/android.cpp (CtrlState::screen_state).
 *
 * The game's own touchscreen keeps working. This view lies over SDL's surface and takes every finger:
 * one that lands on a control is the control's until it lifts; any other is handed to SDL as a touch,
 * exactly as SDLSurface would have (Android gives a view that already holds a finger every further
 * finger inside it, so passing a touch on by declining it would fail while a thumb is on a stick). The
 * game's touch spots (its [touch] bindings' points, such as Soul Sacrifice's Black Rite icon) are kept
 * clear: controls are moved off them, and a finger on one always goes to the game.
 *
 * They hide while a gamepad is used (or is connected when the game starts: a handheld's built-in
 * controls) and come back when the last gamepad is disconnected or the setting is switched on again.
 * A game may ask for a latch button (Profile::screen_latch; ss-delta: LOCK, L for the lock-on): a tap
 * holds its Vita button down until the next tap.
 * Switched off, only the menu button stays, and they all show while the settings menu is open: a phone
 * without a gamepad can always reach the setting again.
 */
public class OnScreenControls extends View {
    // SceCtrlButtons (vita3k/ctrl/include/ctrl/ctrl.h) as the extended functions read them
    static final int SELECT = 0x1, START = 0x8, UP = 0x10, RIGHT = 0x20, DOWN = 0x40, LEFT = 0x80,
        L1 = 0x400, R1 = 0x800, TRIANGLE = 0x1000, CIRCLE = 0x2000, CROSS = 0x4000, SQUARE = 0x8000;

    // what a finger belongs to
    private static final int OWNER_GAME = 0, OWNER_BUTTONS = 1, OWNER_DPAD = 2, OWNER_LEFT_STICK = 3,
        OWNER_RIGHT_STICK = 4, OWNER_MENU = 5, OWNER_NONE = 6, // none: ignored until it lifts
        OWNER_LATCH = 7;

    // A press reaches the game for at least this long: the game reads the buttons once a frame, and a
    // quick tap could otherwise start and end between two reads (adb's taps did)
    private static final long MIN_PRESS_MS = 70;

    // a game touch spot's radius in the Vita's 960x544 points (**hypothesis**: the Black Rite icon's size)
    private static final float SPOT_RADIUS = 36f;

    private static native void setState(int buttons, float lx, float ly, float rx, float ry);
    private static native void toggleMenu();

    private static final int SYMBOL_TEXT = 0, SYMBOL_TRIANGLE = 1, SYMBOL_CIRCLE = 2, SYMBOL_CROSS = 3,
        SYMBOL_SQUARE = 4, SYMBOL_MENU = 5;

    private static final class Button {
        final int bit; // 0: the menu button
        final int symbol;
        final String label;
        final int color;
        final boolean round;
        final RectF rect = new RectF();
        boolean pressed;

        Button(int bit, int symbol, String label, int color, boolean round) {
            this.bit = bit;
            this.symbol = symbol;
            this.label = label;
            this.color = color;
            this.round = round;
        }
    }

    private static final class Stick {
        float homeX, homeY, radius; // where it rests, the base's radius
        float centerX, centerY; // while held: where the finger came down
        float x, y; // -1 to 1, right and down positive
        int pointer = -1;

        void press(int id, float px, float py) {
            pointer = id;
            centerX = px;
            centerY = py;
            x = y = 0;
        }

        void move(float px, float py) {
            float dx = (px - centerX) / radius, dy = (py - centerY) / radius;
            float length = (float) Math.hypot(dx, dy);
            if (length > 1) {
                dx /= length;
                dy /= length;
            }
            x = dx;
            y = dy;
        }

        void release() {
            pointer = -1;
            x = y = 0;
        }
    }

    private final List<Button> buttons = new ArrayList<>();
    private final Button menu, l, r, select, start;
    private Button latch; // the game's latch button, or null
    private boolean latched; // its Vita button is held
    private final RectF dpad = new RectF();
    private int dpadBits;
    private final Stick leftStick = new Stick(), rightStick = new Stick();
    private final SparseIntArray owners = new SparseIntArray();

    private boolean enabled; // [controls] on_screen
    private boolean configured; // configure has been called (the game's first call isn't a switch)
    private boolean hiddenByGamepad;
    private boolean settingsMenuOpen;
    private int sizePercent = 100;
    private float[] spotPoints = new float[0]; // 960x544 points, x, y pairs
    private float[] spots = new float[0]; // on the screen: x, y, radius triples
    private float unit = 1; // a dp at the chosen size

    private int sentButtons;
    private int heldButtons; // what the fingers hold now (sentButtons may keep a short press longer)
    private final long[] pressedAt = new long[16]; // when each button bit last went down
    private final Runnable resend = this::send;
    private float sentLx, sentLy, sentRx, sentRy;

    private final Paint fill = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint stroke = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint symbol = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint text = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Path path = new Path();
    private final RectF block = new RectF();

    public OnScreenControls(Context context) {
        super(context);
        setFocusable(false); // gamepad events stay with SDL's surface
        stroke.setStyle(Paint.Style.STROKE);
        symbol.setStyle(Paint.Style.STROKE);
        symbol.setStrokeCap(Paint.Cap.ROUND);
        symbol.setStrokeJoin(Paint.Join.ROUND);
        text.setColor(Color.WHITE);
        text.setTextAlign(Paint.Align.CENTER);
        text.setTypeface(Typeface.create(Typeface.DEFAULT, Typeface.BOLD));

        // the PlayStation colours of the face buttons' symbols
        buttons.add(new Button(TRIANGLE, SYMBOL_TRIANGLE, null, 0xff40e0b0, true));
        buttons.add(new Button(CIRCLE, SYMBOL_CIRCLE, null, 0xffff6b6b, true));
        buttons.add(new Button(CROSS, SYMBOL_CROSS, null, 0xff8ab4ff, true));
        buttons.add(new Button(SQUARE, SYMBOL_SQUARE, null, 0xfff49ad8, true));
        buttons.add(l = new Button(L1, SYMBOL_TEXT, "L", Color.WHITE, false));
        buttons.add(r = new Button(R1, SYMBOL_TEXT, "R", Color.WHITE, false));
        buttons.add(select = new Button(SELECT, SYMBOL_TEXT, "SELECT", Color.WHITE, false));
        buttons.add(start = new Button(START, SYMBOL_TEXT, "START", Color.WHITE, false));
        buttons.add(menu = new Button(0, SYMBOL_MENU, null, Color.WHITE, true));
    }

    /** [controls] from the game (any thread's call is posted here by GameActivity). */
    public void configure(boolean on, int size, int opacity, float[] points, int latchButton, String latchLabel) {
        if (latchButton == 0 || latchLabel == null || latchLabel.isEmpty())
            latch = null;
        else if (latch == null || latch.bit != latchButton || !latch.label.equals(latchLabel))
            latch = new Button(latchButton, SYMBOL_TEXT, latchLabel, Color.WHITE, false);
        if (latch == null)
            latched = false;
        if (on && !enabled && configured)
            hiddenByGamepad = false; // switched on in the settings menu: shown, gamepad or not
        configured = true;
        enabled = on;
        sizePercent = size;
        spotPoints = points != null ? points : new float[0];
        setAlpha(Math.max(0.1f, Math.min(1f, opacity / 100f)));
        if (!controlsShown())
            releaseControls();
        layoutControls();
        invalidate();
    }

    /** The frontend's settings menu opened or closed. */
    public void setSettingsMenuOpen(boolean open) {
        settingsMenuOpen = open;
        if (!controlsShown())
            releaseControls();
        else if (open && latched) {
            // the menu pages with L and R; a latched shoulder button would keep its press from registering
            latched = false;
            latch.pressed = false;
            send();
        }
        invalidate();
    }

    /** At the first configure: a gamepad is connected (a handheld's built-in controls). */
    public void setHiddenByGamepad(boolean hidden) {
        hiddenByGamepad = hidden;
        if (hidden)
            releaseControls();
        invalidate();
    }

    /** A gamepad's button or stick was used. */
    public void gamepadUsed() {
        if (!hiddenByGamepad)
            setHiddenByGamepad(true);
    }

    /** The last gamepad is gone. */
    public void gamepadsGone() {
        if (hiddenByGamepad)
            setHiddenByGamepad(false);
    }

    // the menu button: unless a gamepad is in use
    private boolean menuButtonShown() {
        return !hiddenByGamepad;
    }

    // everything else: switched on, or while the settings menu is open
    private boolean controlsShown() {
        return !hiddenByGamepad && (enabled || settingsMenuOpen);
    }

    /** Everything held is let go (the app leaves the screen, the controls hide). */
    public void releaseControls() {
        for (Button b : buttons)
            b.pressed = false;
        latched = false;
        if (latch != null)
            latch.pressed = false;
        dpadBits = 0;
        leftStick.release();
        rightStick.release();
        // fingers on the controls are ignored until they lift; the game's keep their touches
        for (int i = 0; i < owners.size(); i++)
            if (owners.valueAt(i) != OWNER_GAME)
                owners.setValueAt(i, OWNER_NONE);
        send();
        invalidate();
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        layoutControls();
    }

    @Override
    public WindowInsets onApplyWindowInsets(WindowInsets insets) {
        WindowInsets result = super.onApplyWindowInsets(insets);
        layoutControls();
        return result;
    }

    // Phone landscape: L above the D-pad above the left stick on the left, R above the face buttons
    // above the right stick on the right, SELECT and START at the bottom's middle, the menu button at
    // the top's. Sizes in dp, shrunk to fit a short screen.
    private void layoutControls() {
        int w = getWidth(), h = getHeight();
        if (w == 0 || h == 0)
            return;
        float density = getResources().getDisplayMetrics().density;
        float u = Math.min(density * sizePercent / 100f, h / 360f);
        unit = u;
        float insetLeft = 0, insetRight = 0;
        WindowInsets insets = getRootWindowInsets();
        DisplayCutout cutout = insets != null ? insets.getDisplayCutout() : null;
        if (cutout != null) {
            // the picture reaches under a camera hole (GameActivity's cutout mode); the controls don't
            insetLeft = cutout.getSafeInsetLeft();
            insetRight = cutout.getSafeInsetRight();
        }
        float margin = 14 * u;
        float left = insetLeft + margin, right = w - insetRight - margin, top = margin, bottom = h - margin;

        l.rect.set(left, top, left + 96 * u, top + 42 * u);
        r.rect.set(right - 96 * u, top, right, top + 42 * u);
        // the latch beside the shoulder button it holds, smaller (ss-delta's lock-on goes with L,
        // Shinobido 2's crouch with R); beside L for any other button
        if (latch != null && latch.bit == R1)
            latch.rect.set(r.rect.left - 88 * u, top + 4 * u, r.rect.left - 12 * u, top + 38 * u);
        else if (latch != null)
            latch.rect.set(l.rect.right + 12 * u, top + 4 * u, l.rect.right + 88 * u, top + 38 * u);

        // the game's touch spots on the screen, where Vita3K draws the picture: 960x544 fitted in the middle
        float scale = Math.min(w / 960f, h / 544f);
        float originX = (w - 960 * scale) / 2, originY = (h - 544 * scale) / 2;
        spots = new float[spotPoints.length / 2 * 3];
        for (int i = 0, j = 0; i + 1 < spotPoints.length; i += 2, j += 3) {
            spots[j] = originX + spotPoints[i] * scale;
            spots[j + 1] = originY + spotPoints[i + 1] * scale;
            spots[j + 2] = SPOT_RADIUS * scale;
        }
        float gap = 6 * u;

        // the sticks in the lower corners, moved straight up (or down) off any spot they cover, so the
        // D-pad and face buttons above them stay in line (Soul Sacrifice's Black Rite icon, lower left)
        float stickRadius = 58 * u;
        leftStick.radius = rightStick.radius = stickRadius;
        leftStick.homeX = left + 70 * u;
        leftStick.homeY = clearVertically(leftStick.homeX, bottom - 70 * u, stickRadius + gap);
        rightStick.homeX = right - 70 * u;
        rightStick.homeY = clearVertically(rightStick.homeX, bottom - 70 * u, stickRadius + gap);

        float dpadHalf = 66 * u;
        float dpadX = left + 70 * u, dpadY = leftStick.homeY - stickRadius - 16 * u - dpadHalf;
        dpad.set(dpadX - dpadHalf, dpadY - dpadHalf, dpadX + dpadHalf, dpadY + dpadHalf);

        float faceRadius = 26 * u, faceOffset = 48 * u;
        float faceX = right - 80 * u, faceY = rightStick.homeY - stickRadius - 12 * u - faceOffset - faceRadius;
        setCircle(buttons.get(0), faceX, faceY - faceOffset, faceRadius); // triangle
        setCircle(buttons.get(1), faceX + faceOffset, faceY, faceRadius); // circle
        setCircle(buttons.get(2), faceX, faceY + faceOffset, faceRadius); // cross
        setCircle(buttons.get(3), faceX - faceOffset, faceY, faceRadius); // square

        float middle = w / 2f;
        select.rect.set(middle - 96 * u, bottom - 30 * u, middle - 16 * u, bottom);
        start.rect.set(middle + 16 * u, bottom - 30 * u, middle + 96 * u, bottom);
        setCircle(menu, middle, top + 20 * u, 20 * u);
        keepSpotsClear(w, h, margin);
    }

    // a stick's y, moved straight away from each spot whose circle its own (radius) would overlap
    private float clearVertically(float x, float y, float radius) {
        for (int s = 0; s + 2 < spots.length; s += 3) {
            float dx = Math.abs(x - spots[s]), reach = radius + spots[s + 2];
            if (dx >= reach)
                continue;
            float dy = (float) Math.sqrt(reach * reach - dx * dx);
            if (Math.abs(y - spots[s + 1]) < dy)
                y = y <= spots[s + 1] ? spots[s + 1] - dy : spots[s + 1] + dy;
        }
        return y;
    }

    private static void setCircle(Button b, float x, float y, float radius) {
        b.rect.set(x - radius, y - radius, x + radius, y + radius);
    }

    // Each other control that covers a touch spot moves straight away from it until it no longer does,
    // staying on the screen (the sticks were placed so already).
    private void keepSpotsClear(int w, int h, float margin) {
        float gap = 6 * unit;
        for (int pass = 0; pass < 3; pass++) {
            for (int s = 0; s + 2 < spots.length; s += 3) {
                float sx = spots[s], sy = spots[s + 1], sr = spots[s + 2];
                float half = dpad.width() / 2;
                float[] c = pushAway(dpad.centerX(), dpad.centerY(), half * 1.2f, sx, sy, sr + gap);
                float dx = clamp(c[0], margin + half, w - margin - half), dy = clamp(c[1], margin + half, h - margin - half);
                dpad.set(dx - half, dy - half, dx + half, dy + half);
                List<Button> all = new ArrayList<>(buttons);
                if (latch != null)
                    all.add(latch);
                for (Button b : all) {
                    float bw = b.rect.width() / 2, bh = b.rect.height() / 2;
                    c = pushAway(b.rect.centerX(), b.rect.centerY(), Math.max(bw, bh), sx, sy, sr + gap);
                    float bx = clamp(c[0], margin + bw, w - margin - bw), by = clamp(c[1], margin + bh, h - margin - bh);
                    b.rect.set(bx - bw, by - bh, bx + bw, by + bh);
                }
            }
        }
    }

    private static float[] pushAway(float x, float y, float radius, float sx, float sy, float sr) {
        float dx = x - sx, dy = y - sy;
        float distance = (float) Math.hypot(dx, dy);
        float need = radius + sr - distance;
        if (need <= 0)
            return new float[] { x, y };
        if (distance < 0.001f) {
            dx = 0;
            dy = -1;
            distance = 1;
        }
        return new float[] { x + dx / distance * need, y + dy / distance * need };
    }

    private static float clamp(float v, float lo, float hi) {
        return lo > hi ? (lo + hi) / 2 : Math.max(lo, Math.min(hi, v));
    }

    private boolean onSpot(float x, float y) {
        for (int s = 0; s + 2 < spots.length; s += 3)
            if (Math.hypot(x - spots[s], y - spots[s + 1]) <= spots[s + 2])
                return true;
        return false;
    }

    // what a finger coming down at x, y belongs to
    private int claim(float x, float y) {
        if (!menuButtonShown() || onSpot(x, y))
            return OWNER_GAME;
        float slack = 8 * unit;
        if (contains(menu.rect, x, y, slack))
            return OWNER_MENU;
        if (!controlsShown())
            return OWNER_GAME;
        if (latch != null && contains(latch.rect, x, y, slack))
            return OWNER_LATCH;
        if (buttonAt(x, y, slack) != null)
            return OWNER_BUTTONS;
        if (contains(dpad, x, y, slack))
            return OWNER_DPAD;
        // a stick's area is larger than its base; a second finger there while it is held does nothing
        if (Math.hypot(x - leftStick.homeX, y - leftStick.homeY) <= leftStick.radius * 1.5f)
            return leftStick.pointer < 0 ? OWNER_LEFT_STICK : OWNER_NONE;
        if (Math.hypot(x - rightStick.homeX, y - rightStick.homeY) <= rightStick.radius * 1.5f)
            return rightStick.pointer < 0 ? OWNER_RIGHT_STICK : OWNER_NONE;
        return OWNER_GAME;
    }

    private static boolean contains(RectF rect, float x, float y, float slack) {
        return x >= rect.left - slack && x <= rect.right + slack && y >= rect.top - slack && y <= rect.bottom + slack;
    }

    private Button buttonAt(float x, float y, float slack) {
        for (Button b : buttons) {
            if (b == menu)
                continue;
            if (b.round) {
                if (Math.hypot(x - b.rect.centerX(), y - b.rect.centerY()) <= b.rect.width() / 2 + slack)
                    return b;
            } else if (contains(b.rect, x, y, slack))
                return b;
        }
        return null;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();
        if (action == MotionEvent.ACTION_DOWN)
            owners.clear();
        // hidden, and no finger of ours down: SDL's surface below takes the touches itself
        if (!menuButtonShown() && owners.size() == 0 && action == MotionEvent.ACTION_DOWN)
            return false;

        int index = event.getActionIndex();
        switch (action) {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_POINTER_DOWN: {
            int id = event.getPointerId(index);
            float x = event.getX(index), y = event.getY(index);
            int owner = claim(x, y);
            owners.put(id, owner);
            if (owner == OWNER_GAME)
                forward(event, index, MotionEvent.ACTION_DOWN);
            else if (owner == OWNER_MENU) {
                menu.pressed = true;
                performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
                toggleMenu();
            } else if (owner == OWNER_LATCH) {
                latched = !latched;
                latch.pressed = latched;
                performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
            } else if (owner == OWNER_LEFT_STICK)
                leftStick.press(id, x, y);
            else if (owner == OWNER_RIGHT_STICK)
                rightStick.press(id, x, y);
            break;
        }
        case MotionEvent.ACTION_MOVE:
            for (int i = 0; i < event.getPointerCount(); i++) {
                int owner = owners.get(event.getPointerId(i), -1);
                if (owner == OWNER_GAME)
                    forward(event, i, MotionEvent.ACTION_MOVE);
                else if (owner == OWNER_LEFT_STICK)
                    leftStick.move(event.getX(i), event.getY(i));
                else if (owner == OWNER_RIGHT_STICK)
                    rightStick.move(event.getX(i), event.getY(i));
            }
            break;
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP: {
            int id = event.getPointerId(index);
            int owner = owners.get(id, -1);
            if (owner == OWNER_GAME)
                forward(event, index, MotionEvent.ACTION_UP);
            else if (owner == OWNER_MENU)
                menu.pressed = false;
            else if (owner == OWNER_LEFT_STICK)
                leftStick.release();
            else if (owner == OWNER_RIGHT_STICK)
                rightStick.release();
            owners.delete(id);
            break;
        }
        case MotionEvent.ACTION_CANCEL:
            for (int i = 0; i < event.getPointerCount(); i++)
                if (owners.get(event.getPointerId(i), -1) == OWNER_GAME)
                    forward(event, i, MotionEvent.ACTION_CANCEL);
            owners.clear();
            menu.pressed = false;
            leftStick.release();
            rightStick.release();
            break;
        default:
            break;
        }
        updateHeld(event, action);
        return true;
    }

    // the buttons and D-pad directions under the fingers that hold them (a finger slides from one button
    // to the next), the lifting finger left out
    private void updateHeld(MotionEvent event, int action) {
        int lifted = action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_UP ? event.getActionIndex() : -1;
        boolean all = action == MotionEvent.ACTION_CANCEL;
        int held = 0, dpadHeld = 0;
        for (int i = 0; i < event.getPointerCount() && !all; i++) {
            if (i == lifted)
                continue;
            int owner = owners.get(event.getPointerId(i), -1);
            float x = event.getX(i), y = event.getY(i);
            if (owner == OWNER_BUTTONS) {
                Button b = buttonAt(x, y, 4 * unit);
                if (b != null)
                    held |= b.bit;
            } else if (owner == OWNER_DPAD)
                dpadHeld |= dpadDirections(x, y);
        }
        int before = 0;
        for (Button b : buttons)
            if (b.pressed && b != menu)
                before |= b.bit;
        before |= dpadBits;
        for (Button b : buttons)
            if (b != menu)
                b.pressed = (held & b.bit) != 0;
        dpadBits = dpadHeld;
        if (((held | dpadHeld) & ~before) != 0)
            performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
        send();
        invalidate();
    }

    // eight ways: a direction counts when the finger is within 67.5 degrees of it
    private int dpadDirections(float x, float y) {
        float dx = x - dpad.centerX(), dy = y - dpad.centerY();
        float ax = Math.abs(dx), ay = Math.abs(dy);
        if (Math.hypot(dx, dy) < dpad.width() * 0.12f)
            return 0;
        int bits = 0;
        if (ax > ay * 0.4142f)
            bits |= dx > 0 ? RIGHT : LEFT;
        if (ay > ax * 0.4142f)
            bits |= dy > 0 ? DOWN : UP;
        return bits;
    }

    // a finger that isn't on a control: to SDL as a touch, as SDLSurface.onTouch sends it
    private void forward(MotionEvent event, int index, int action) {
        float w = Math.max(1, getWidth() - 1), h = Math.max(1, getHeight() - 1);
        float pressure = Math.min(1f, event.getPressure(index));
        SDLActivity.onNativeTouch(event.getDeviceId(), event.getPointerId(index), action,
            event.getX(index) / w, event.getY(index) / h, pressure);
    }

    private void send() {
        int held = dpadBits;
        for (Button b : buttons)
            if (b.pressed)
                held |= b.bit;
        if (latch != null && latched)
            held |= latch.bit;
        long now = SystemClock.uptimeMillis();
        for (int i = 0; i < 16; i++)
            if ((held & ~heldButtons & (1 << i)) != 0)
                pressedAt[i] = now;
        heldButtons = held;
        // a press let go too soon stays down until its time is up
        long wait = 0;
        for (int i = 0; i < 16; i++) {
            long left = pressedAt[i] + MIN_PRESS_MS - now;
            if ((sentButtons & ~held & (1 << i)) != 0 && left > 0) {
                held |= 1 << i;
                wait = Math.max(wait, left);
            }
        }
        removeCallbacks(resend);
        if (wait > 0)
            postDelayed(resend, wait);
        if (held == sentButtons && leftStick.x == sentLx && leftStick.y == sentLy && rightStick.x == sentRx && rightStick.y == sentRy)
            return;
        sentButtons = held;
        sentLx = leftStick.x;
        sentLy = leftStick.y;
        sentRx = rightStick.x;
        sentRy = rightStick.y;
        setState(held, sentLx, sentLy, sentRx, sentRy);
    }

    // drawing: dark translucent shapes with white outlines, lit while held; the view's alpha is the opacity
    @Override
    protected void onDraw(Canvas canvas) {
        if (!menuButtonShown())
            return;
        float u = unit;
        stroke.setStrokeWidth(2 * u);
        drawButton(canvas, menu, u);
        if (!controlsShown())
            return;
        for (Button b : buttons)
            if (b != menu)
                drawButton(canvas, b, u);
        if (latch != null)
            drawButton(canvas, latch, u);
        drawDpad(canvas, u);
        drawStick(canvas, leftStick, u);
        drawStick(canvas, rightStick, u);
    }

    private void setFill(boolean pressed) {
        fill.setColor(pressed ? 0xa0ffffff : 0x66000000);
        stroke.setColor(pressed ? 0xffffffff : 0xccffffff);
    }

    private void drawButton(Canvas canvas, Button b, float u) {
        setFill(b.pressed);
        float cx = b.rect.centerX(), cy = b.rect.centerY();
        if (b.round) {
            float radius = b.rect.width() / 2;
            canvas.drawCircle(cx, cy, radius, fill);
            canvas.drawCircle(cx, cy, radius, stroke);
        } else {
            float corner = b.rect.height() / 2;
            canvas.drawRoundRect(b.rect, corner, corner, fill);
            canvas.drawRoundRect(b.rect, corner, corner, stroke);
        }
        int color = b.pressed && b.symbol != SYMBOL_TEXT && b.symbol != SYMBOL_MENU ? 0xff202020 : b.color;
        float s = b.rect.height() * 0.24f; // a symbol's half size
        symbol.setColor(color);
        symbol.setStrokeWidth(Math.max(2 * u, b.rect.height() * 0.07f));
        switch (b.symbol) {
        case SYMBOL_TRIANGLE:
            path.reset();
            path.moveTo(cx, cy - s * 1.05f);
            path.lineTo(cx + s * 1.1f, cy + s * 0.85f);
            path.lineTo(cx - s * 1.1f, cy + s * 0.85f);
            path.close();
            canvas.drawPath(path, symbol);
            break;
        case SYMBOL_CIRCLE:
            canvas.drawCircle(cx, cy, s, symbol);
            break;
        case SYMBOL_CROSS:
            canvas.drawLine(cx - s, cy - s, cx + s, cy + s, symbol);
            canvas.drawLine(cx - s, cy + s, cx + s, cy - s, symbol);
            break;
        case SYMBOL_SQUARE:
            canvas.drawRect(cx - s * 0.9f, cy - s * 0.9f, cx + s * 0.9f, cy + s * 0.9f, symbol);
            break;
        case SYMBOL_MENU:
            for (int i = -1; i <= 1; i++)
                canvas.drawLine(cx - s, cy + i * s * 0.75f, cx + s, cy + i * s * 0.75f, symbol);
            break;
        default:
            text.setColor(b.pressed ? 0xff202020 : Color.WHITE);
            text.setTextSize(b.label.length() > 1 ? b.rect.height() * 0.42f : b.rect.height() * 0.55f);
            canvas.drawText(b.label, cx, cy - (text.descent() + text.ascent()) / 2, text);
            break;
        }
    }

    private void drawDpad(Canvas canvas, float u) {
        float cx = dpad.centerX(), cy = dpad.centerY(), half = dpad.width() / 2, arm = half / 3;
        int[] bits = { UP, RIGHT, DOWN, LEFT };
        for (int i = 0; i < 4; i++) {
            canvas.save();
            canvas.rotate(90 * i, cx, cy);
            // the up arm, turned into place: a rounded block and its arrow
            boolean pressed = (dpadBits & bits[i]) != 0;
            setFill(pressed);
            block.set(cx - arm, cy - half, cx + arm, cy - arm * 0.15f);
            canvas.drawRoundRect(block, arm * 0.35f, arm * 0.35f, fill);
            canvas.drawRoundRect(block, arm * 0.35f, arm * 0.35f, stroke);
            path.reset();
            float tip = cy - half + arm * 0.55f;
            path.moveTo(cx, tip);
            path.lineTo(cx + arm * 0.5f, tip + arm * 0.6f);
            path.lineTo(cx - arm * 0.5f, tip + arm * 0.6f);
            path.close();
            fill.setColor(pressed ? 0xff202020 : 0xddffffff);
            canvas.drawPath(path, fill);
            canvas.restore();
        }
    }

    private void drawStick(Canvas canvas, Stick stick, float u) {
        boolean held = stick.pointer >= 0;
        float bx = held ? stick.centerX : stick.homeX, by = held ? stick.centerY : stick.homeY;
        setFill(false);
        canvas.drawCircle(bx, by, stick.radius, fill);
        canvas.drawCircle(bx, by, stick.radius, stroke);
        float kx = bx + stick.x * stick.radius, ky = by + stick.y * stick.radius;
        setFill(held);
        canvas.drawCircle(kx, ky, stick.radius * 0.45f, fill);
        canvas.drawCircle(kx, ky, stick.radius * 0.45f, stroke);
    }
}
