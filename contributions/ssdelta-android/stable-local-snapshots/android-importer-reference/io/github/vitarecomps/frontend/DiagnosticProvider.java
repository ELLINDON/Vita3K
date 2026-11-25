package io.github.vitarecomps.frontend;
import android.content.*;import android.database.*;import android.net.Uri;import android.os.*;import android.provider.OpenableColumns;import java.io.*;
public class DiagnosticProvider extends ContentProvider {
 public boolean onCreate(){return true;}public String getType(Uri u){return "text/plain";}
 public ParcelFileDescriptor openFile(Uri u,String mode)throws FileNotFoundException{if(!"r".equals(mode)||u.getLastPathSegment()==null||!u.getLastPathSegment().startsWith("SSD_"))throw new FileNotFoundException("Relatorio indisponivel");return ParcelFileDescriptor.open(ImportDiagnostics.reportFile(getContext()),ParcelFileDescriptor.MODE_READ_ONLY);}
 public Cursor query(Uri u,String[] p,String sel,String[] args,String sort){File f=ImportDiagnostics.reportFile(getContext());MatrixCursor c=new MatrixCursor(new String[]{OpenableColumns.DISPLAY_NAME,OpenableColumns.SIZE});c.addRow(new Object[]{u.getLastPathSegment(),f.length()});return c;}
 public Uri insert(Uri u,ContentValues v){throw new UnsupportedOperationException();}public int delete(Uri u,String s,String[] a){throw new UnsupportedOperationException();}public int update(Uri u,ContentValues v,String s,String[] a){throw new UnsupportedOperationException();}
}
