package io.github.vitarecomps.frontend;
import android.app.Activity;import android.os.Bundle;import android.content.Intent;import android.widget.Toast;
public final class DriverPickerActivity extends Activity {
 protected void onCreate(Bundle state){super.onCreate(state);if(state==null){Intent i=new Intent(Intent.ACTION_OPEN_DOCUMENT);i.setType("*/*");i.addCategory(Intent.CATEGORY_OPENABLE);try{startActivityForResult(i,DriverImport.REQUEST);}catch(Exception e){Toast.makeText(this,"Nao foi possivel abrir arquivos",Toast.LENGTH_LONG).show();finish();}}}
 protected void onActivityResult(int request,int code,Intent data){super.onActivityResult(request,code,data);if(request==DriverImport.REQUEST){if(code==RESULT_OK&&data!=null&&data.getData()!=null){DriverImport.result(this,request,code,data);}else finish();}}
}