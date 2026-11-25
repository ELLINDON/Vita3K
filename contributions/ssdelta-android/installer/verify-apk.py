from pathlib import Path
import subprocess,zipfile,hashlib,json,io
b=Path(r'E:\GAMES\SOUL SACRIFICE DELTA\android-teste');p=b/'apk-progresso-integrado';w=b/'importador-corrigido';t=w/'tools/build-tools/android-15';java=Path(r'C:\Program Files\Microsoft\jdk-17.0.20.101-hotspot\bin\java.exe')
def run(args):subprocess.run([str(a) for a in args],check=True)
apk=p/'ss-delta-progresso-integrado-130-teste.apk'
run([t/'zipalign.exe','-P','16','-f','4',p/'unsigned.apk',p/'aligned.apk'])
run([java,'-jar',t/'lib/apksigner.jar','sign','--ks',w/'importfix-test.jks','--ks-pass','pass:android','--key-pass','pass:android','--out',apk,p/'aligned.apk'])
run([java,'-jar',t/'lib/apksigner.jar','verify','--verbose',apk]);run([t/'zipalign.exe','-c','-P','16','4',apk])
with zipfile.ZipFile(b/'driver-recuperacao/ss-delta-driver-recuperacao-teste.apk') as original,zipfile.ZipFile(apk) as z:
 libs=[n for n in original.namelist() if n.startswith('lib/') and not n.endswith('/')]
 for name in libs:assert original.read(name)==z.read(name),name
 with zipfile.ZipFile(io.BytesIO(z.read('assets/buildtools.zip'))) as k:
  assert not any(n.startswith('precompiled/') or n.lower().endswith(('.pup','.elf','.pkg')) or '/generated/' in n for n in k.namelist())
  game=json.loads(k.read('game.json'));assert all(v.startswith('1.30') for v in game['eboot_sha256'].values())
  assert k.read('kit/tools/android-setup.py')==(p/'source/android-setup.py').read_bytes()
  count=sum(not i.is_dir() for i in k.infolist())
 meta=json.loads(z.read('assets/buildtools.json'));assert count==meta['files'];assert hashlib.sha256(z.read('assets/buildtools.zip')).hexdigest()==meta['zip_sha256']
manifest=(p/'decoded/AndroidManifest.xml').read_text();assert manifest.count('sensorLandscape')==7
assert 'Theme.Translucent.NoTitleBar.Fullscreen' in manifest
source=(p/'source/io/github/vitarecomps/frontend/ImportScreen.java').read_text();assert 'Only Soul Sacrifice Delta version 1.30 is supported.' in source;assert '0xff268f87' in source
r={'apk':str(apk),'bytes':apk.stat().st_size,'sha256':hashlib.sha256(apk.read_bytes()).hexdigest(),'native_libraries_unchanged':len(libs),'landscape_preserved':True,'supported_game':'1.30 only (existing eboot hash validation retained)','game_firmware_modules_embedded':0,'change':'native extraction activity translucent/invisible; actual setup screen displays progress; native module dump still visible for on-device compilation','tests':'Java/DEX/APK assembly, six log-progress checks, original native entry/lifecycle integration, game-hash policy, signature and alignment','device_validation':'pending: visual continuity, input locking and full install must be confirmed on Android; previous separate-screen APK completed installation on user device'}
(p/'RESULTADO.json').write_text(json.dumps(r,indent=2));print('READY:',apk);print('SIZE:',apk.stat().st_size)
