package io.github.vitarecomps.frontend;
import android.app.Activity;
import android.view.*;
import android.widget.*;
public final class GameDiagnostics {
 private static final String TAG="ssd-share-diagnostic";
 public static void menu(Activity a,boolean open){a.runOnUiThread(()->{
   ViewGroup content=a.findViewById(android.R.id.content);
   if(content==null)return;
   View previous=content.findViewWithTag(TAG);
   if(!open){if(previous!=null&&previous.getParent() instanceof ViewGroup)((ViewGroup)previous.getParent()).removeView(previous);return;}
   if(previous!=null)return;
   Button button=new Button(a);button.setTag(TAG);button.setText("Compartilhar diagnostico");button.setTextSize(14);
   button.setOnClickListener(v->{try{ImportDiagnostics.share(a);}catch(Exception e){Toast.makeText(a,"Nao foi possivel compartilhar: "+e.getMessage(),Toast.LENGTH_LONG).show();}});
   int margin=(int)(12*a.getResources().getDisplayMetrics().density);
   FrameLayout.LayoutParams params=new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT,ViewGroup.LayoutParams.WRAP_CONTENT,Gravity.TOP|Gravity.LEFT);
   params.leftMargin=margin;params.topMargin=margin;
   LinearLayout box=new LinearLayout(a);box.setTag(TAG);button.setTag(null);box.addView(button);Button driver=new Button(a);driver.setText("Importar driver ZIP");driver.setTextSize(14);driver.setOnClickListener(v->DriverImport.pick(a));box.addView(driver);a.addContentView(box,params);box.bringToFront();
 });}
}