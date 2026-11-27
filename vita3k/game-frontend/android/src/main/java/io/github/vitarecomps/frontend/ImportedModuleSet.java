package io.github.vitarecomps.frontend;
import android.content.Context; import java.io.*; import org.json.JSONObject;
final class ImportedModuleSet {
 static boolean reusable(Context context, JSONObject record) {
  if (!"User-imported ARM64".equals(record.optString("source"))) return false;
  JSONObject modules=record.optJSONObject("modules"); if(modules==null)return false;
  File root=BuildActivity.gameDir(context);
  try { for(String name:ArmModuleFiles.NAMES){
   JSONObject entry=modules.optJSONObject(name);File file=new File(root,name);
   if(entry==null||!file.isFile()||file.length()<64||file.length()!=entry.optLong("bytes",-1)||!entry.optString("sha256").matches("[0-9a-fA-F]{64}"))return false;
   try(RandomAccessFile in=new RandomAccessFile(file,"r")){byte[] h=new byte[20];in.readFully(h);if(h[0]!=127||h[1]!=69||h[2]!=76||h[3]!=70||h[4]!=2||h[5]!=1||h[16]!=3||h[17]!=0||(h[18]&255)!=183||h[19]!=0)return false;}
  } return true; } catch(IOException error){return false;}
 }
}
