from pathlib import Path
import hashlib,json,zipfile,tempfile,types,sys,os,shutil
p=Path(__file__).resolve().parent.parent
source=(p/'source/android-setup.py').read_text(); code=source[source.index('def user_import_root():'):source.index('def main():')]
root=Path(tempfile.mkdtemp(prefix='ssdelta-offline-tests-'));kit=root/'buildtools/kit';kit.mkdir(parents=True)
checks=[]
def fail(text):raise RuntimeError(text)
env={'KIT':str(kit),'TOOLS':str(kit/'tools'),'os':os,'shutil':shutil,'json':json,'hashlib':hashlib,'check':lambda a,p:None,'say':lambda *x:None,'fail':fail,'build_on_device':lambda a,p:'original compiler selected'}
exec(compile(code,'offline_setup.py','exec'),env)
project=types.SimpleNamespace(modules={str(i):n.removeprefix('ssd_aot_').removesuffix('.so') for i,n in enumerate(json.loads((p.parent/'arm64-pc/modulos-arm64.json').read_text()))},installed_title=lambda fs:'TEST')
a=types.SimpleNamespace(game=str(root/'game'),work=str(root/'work'),stamp='test');Path(a.game).mkdir();Path(a.work).mkdir()
try:
 assert env['build'](a,project)=='original compiler selected';checks.append('without imported modules Build delegates to original compiler')
 def archive(bad=False):
  header=bytearray(64);header[:7]=b'\x7fELF\x02\x01\x01';header[16]=3;header[18]=183;data=bytes(header)
  records={'ssd_aot_'+n+'.so':{'bytes':64,'sha256':hashlib.sha256(data).hexdigest()} for n in project.modules.values()}
  if bad:next(iter(records.values()))['sha256']='0'*64
  with zipfile.ZipFile(root/'arm64-import.zip','w') as z:
   z.writestr('modules.json',json.dumps(records))
   for name in records:z.writestr(name,data)
 archive();env['build'](a,project);assert len(list(Path(a.game).glob('ssd_aot_*.so')))==10;checks.append('import installs ten validated modules')
 before={f.name:f.read_bytes() for f in Path(a.game).glob('*.so')};archive(True)
 try:env['build'](a,project);raise AssertionError('Bad digest accepted')
 except RuntimeError:assert before=={f.name:f.read_bytes() for f in Path(a.game).glob('*.so')};checks.append('bad digest leaves installed modules intact')
 tools=Path(env['TOOLS']);tools.mkdir()
 fake='''import os,hashlib\nFIRMWARE=[('PSVUPDAT.PUP','unused',3,hashlib.sha256(b'one').hexdigest()),('PSP2UPDAT.PUP','unused',3,hashlib.sha256(b'two').hexdigest())]\ndef sha256(path):return hashlib.sha256(open(path,'rb').read()).hexdigest()\ndef fetch(dest):raise AssertionError('Network download attempted')\n'''
 (tools/'fetch-firmware.py').write_text(fake)
 fw=root/'offline-firmware';fw.mkdir();(fw/'PSVUPDAT.PUP').write_bytes(b'one');(fw/'PSP2UPDAT.PUP').write_bytes(b'two')
 env['firmware'](a);assert (Path(a.work)/'firmware/PSP2UPDAT.PUP').read_bytes()==b'two';checks.append('offline files validated without network fallback')
 (fw/'PSP2UPDAT.PUP').unlink()
 try:env['firmware'](a);raise AssertionError('Missing offline font accepted')
 except RuntimeError:checks.append('incomplete offline set fails without download')
 print('PASS:', '; '.join(checks))
 (p/'setup-tests.log').write_text('PASS\n'+'\n'.join(checks))
finally:shutil.rmtree(root)
