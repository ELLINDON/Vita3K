package io.github.vitarecomps.frontend;
public final class DPadControl {
 public static int directions(float dx,float dy,float width){float ax=Math.abs(dx),ay=Math.abs(dy);if(Math.hypot(dx,dy)<width*.12f)return 0;int bits=0;if(ax>ay*.4142f)bits|=dx>0?0x20:0x80;if(ay>ax*.4142f)bits|=dy>0?0x40:0x10;return bits;}
}