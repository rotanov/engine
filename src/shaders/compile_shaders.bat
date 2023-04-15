set sh=..\..\bin\shaderc.exe
set i=..\..\3rd\bgfx.cmake\bgfx\src
rm vs.bin.h fs.bin.h &
%sh% -i %i% -f vs.sc -o t0 --type vertex --platform windows --bin2c vs_dx9 -p s_3_0 -O 3  &
%sh% -i %i% -f vs.sc -o t1 --type vertex --platform windows --bin2c vs_dx11 -p s_5_0 -O 3 &
%sh% -i %i% -f vs.sc -o t2 --type vertex --platform linux --bin2c vs_glsl &
%sh% -i %i% -f vs.sc -o t3 --type vertex --platform android --bin2c vs_essl &
%sh% -i %i% -f vs.sc -o t4 --type vertex --platform linux -p spirv --bin2c vs_spv &
cat t0 t1 t2 t3 t4 >> vs.bin.h &
rm t0 t1 t2 t3 t4 &
%sh% -i %i% -f fs.sc -o t0 --type fragment --platform windows --bin2c fs_dx9 -p s_3_0 -O 3 &
%sh% -i %i% -f fs.sc -o t1 --type fragment --platform windows --bin2c fs_dx11 -p s_5_0 -O 3 &
%sh% -i %i% -f fs.sc -o t2 --type fragment --platform linux --bin2c fs_glsl &
%sh% -i %i% -f fs.sc -o t3 --type fragment --platform android --bin2c fs_essl &
%sh% -i %i% -f fs.sc -o t4 --type fragment --platform linux -p spirv --bin2c fs_spv &
cat t0 t1 t2 t3 t4 >> fs.bin.h &
rm t0 t1 t2 t3 t4