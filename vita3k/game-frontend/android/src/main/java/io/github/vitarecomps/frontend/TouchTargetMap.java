package io.github.vitarecomps.frontend;
/** Same centered 960x544 fit used by native touch spots in OnScreenControls. */
public final class TouchTargetMap {
 public static float safe(float v){return Float.isFinite(v)?Math.max(0,Math.min(1,v)):.5f;}
 private static float scale(float w,float h){return Math.min(Math.max(1,w)/960f,Math.max(1,h)/544f);}
 public static float screenX(float x,float w,float h){float s=scale(w,h);return ((w-960*s)/2+safe(x)*960*s)/Math.max(1,w);}
 public static float screenY(float y,float w,float h){float s=scale(w,h);return ((h-544*s)/2+safe(y)*544*s)/Math.max(1,h);}
 public static float vitaX(float x,float w,float h){float s=scale(w,h);return (x-(w-960*s)/2)/(960*s);}
 public static float vitaY(float y,float w,float h){float s=scale(w,h);return (y-(h-544*s)/2)/(544*s);}
}
