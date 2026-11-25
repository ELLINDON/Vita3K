from pathlib import Path
import subprocess,zipfile,json,hashlib,shutil,re
b=Path(r'E:\GAMES\SOUL SACRIFICE DELTA\android-teste');p=b/'apk-progresso-integrado';w=b/'importador-corrigido';tools=w/'tools/build-tools/android-15';java=Path(r'C:\Program Files\Microsoft\jdk-17.0.20.101-hotspot\bin\java.exe')
def run(args):subprocess.run([str(x) for x in args],check=True)
(p/'dex').mkdir(exist_ok=True)
classes=[f for f in (p/'classes').rglob('*.class') if 'Test' not in f.name]
run([java,'-cp',tools/'lib/d8.jar','com.android.tools.r8.D8','--min-api','28','--lib',r'C:\Users\SrSpa\AppData\Local\Android\Sdk\platforms\android-35\android.jar','--output',p/'dex',*classes])
base=b/'driver-recuperacao/ss-delta-driver-recuperacao-teste.apk'
with zipfile.ZipFile(base) as src,zipfile.ZipFile(p/'ui-mini.apk','w') as out:
 for name in ['AndroidManifest.xml','resources.arsc']:out.writestr(name,src.read(name))
 out.write(p/'dex/classes.dex','classes.dex')
run([java,'-jar',w/'tools/apktool.jar','d','-r','-f',p/'ui-mini.apk','-o',p/'ui-decoded'])
for f in (p/'ui-decoded/smali/io/github/vitarecomps/frontend').glob('*.smali'):
 shutil.copy2(f,p/'decoded/smali/io/github/vitarecomps/frontend'/f.name)
# Update only the setup script in the module-free tools archive.
asset=p/'decoded/assets/buildtools.zip';temp=p/'buildtools.updated.zip'
with zipfile.ZipFile(asset) as src,zipfile.ZipFile(temp,'w',zipfile.ZIP_DEFLATED) as out:
 for info in src.infolist():out.writestr(info,(p/'source/android-setup.py').read_bytes() if info.filename=='kit/tools/android-setup.py' else src.read(info))
temp.replace(asset)
meta=json.loads((p/'decoded/assets/buildtools.json').read_text());meta['zip_sha256']=hashlib.sha256(asset.read_bytes()).hexdigest();(p/'decoded/assets/buildtools.json').write_text(json.dumps(meta,indent=1))
# Validate the exact smali integration points before assembly.
activity=(p/'decoded/smali/io/github/vitarecomps/frontend/BuildActivity.smali').read_text();native=(p/'decoded/smali/io/github/vitarecomps/frontend/FrontendStepActivity.smali').read_text()
assert activity.count('->importedReady(Landroid/content/Context;)Z')==1
assert re.search(r'const/4 v7, 0x6\s+iput v7, p0, .*?->step:I', activity)
assert native.count('->prepareWindow(Landroid/app/Activity;)V')==1
assert native.count('->attach(Landroid/app/Activity;)V')==1
assert native.count('->nativeRunMain(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Object;)I')==1
run([java,'-jar',w/'tools/apktool.jar','b',p/'decoded','-o',p/'unsigned.apk'])
