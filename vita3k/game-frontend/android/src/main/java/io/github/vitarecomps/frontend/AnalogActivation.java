package io.github.vitarecomps.frontend;
/** Activation uses the saved anchor, never the moving floating base. */
public final class AnalogActivation {
 public static float reach(float radius,float marginDp,float density,float scale){return (radius+Math.max(0,Math.min(80,marginDp))*density)*Math.max(1,Math.min(3,Float.isFinite(scale)?scale:1.5f));}
 public static boolean contains(float x,float y,float homeX,float homeY,float radius,float marginDp,float density,float scale){
  if(!Float.isFinite(x)||!Float.isFinite(y)||!Float.isFinite(radius)||!Float.isFinite(marginDp)||!Float.isFinite(density)||radius<0||density<=0)return false;
  double reach=reach(radius,marginDp,density,scale);
  return Math.hypot(x-homeX,y-homeY)<=reach;
 }
}
