package io.github.vitarecomps.frontend;
import java.io.*;import java.nio.file.*;
public class InstallProgressLogTest {
 static void require(boolean ok){if(!ok)throw new AssertionError();}
 public static void main(String[] args)throws Exception{
  File f=File.createTempFile("progress", ".log");
  try{
   Files.write(f.toPath(),"old Installation: 100%\n".getBytes());InstallProgressLog r=new InstallProgressLog(f,1);r.poll(1);require(r.percent==-1);
   Files.write(f.toPath(),"Installing picked-0.pkg\nInstallation: 20%\n".getBytes(),StandardOpenOption.APPEND);r.poll(1);require(r.percent==20);
   Files.write(f.toPath(),"Installation: 30".getBytes(),StandardOpenOption.APPEND);r.poll(1);require(r.percent==20);Files.write(f.toPath(),"%\n".getBytes(),StandardOpenOption.APPEND);r.poll(1);require(r.percent==30);
   Files.write(f.toPath(),"Installing firmware x\nFirmware installation: 7%\n".getBytes());r.poll(2);require(r.percent==7);
   r.accept("Installation: 999%\n");require(r.percent==7);
   r.accept("Installing firmware next\n");require(r.percent==-1);
   System.out.println("PASS: 6 progress checks (old log ignored, appended %, partial line, rotation, invalid %, next PUP reset)");
  }finally{f.delete();}
 }
}
