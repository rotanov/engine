set sh=..\..\3rd\bgfx\tools\bin\windows\shaderc.exe
set i=..\..\3rd\bgfx\src
rm vs.bin.h fs.bin.h &
%sh% -i %i% -f vs.sc -o t0 --type v --platform windows --bin2c vs_dx9 -p vs_3_0 &
%sh% -i %i% -f vs.sc -o t1 --type v --platform windows --bin2c vs_dx11 -p vs_5_0 &
%sh% -i %i% -f vs.sc -o t2 --type v --platform linux --bin2c vs_glsl &
cat t0 t1 t2 >> vs.bin.h &
rm t0 t1 t2 &
%sh% -i %i% -f fs.sc -o t0 --type f --platform windows --bin2c fs_dx9 -p ps_3_0 &
%sh% -i %i% -f fs.sc -o t1 --type f --platform windows --bin2c fs_dx11 -p ps_5_0 &
%sh% -i %i% -f fs.sc -o t2 --type f --platform linux --bin2c fs_glsl &
cat t0 t1 t2 >> fs.bin.h &
rm t0 t1 t2
