package io.github.vitarecomps.frontend;
public final class AnalogControl {
 public float x,y;
 public void move(float dx,float dy,float radius){if(!(radius>0)||!Float.isFinite(dx)||!Float.isFinite(dy)){release();return;}x=dx/radius;y=dy/radius;float length=(float)Math.hypot(x,y);if(length>1){x/=length;y/=length;}}
 public void release(){x=y=0;}
}