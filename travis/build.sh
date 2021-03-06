if $CU_TEST
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILD_CASTOR3D=OFF -DCASTOR_BUILDGRP_TEST=ON -DCASTOR_BUILD_TEST_CASTORUTILS=ON
	make -j4
	make test
elif $CU_IMPORTERS
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=ON -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
elif $CU_DIVIDERS
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=ON -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
elif $CU_POSTFXS
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=ON -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
elif $CU_TONEMAPS
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=ON -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
elif $CU_GENERICS
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=ON -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
elif $CU_PARTICLES
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=ON -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
elif $CU_GENERATORS
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=ON -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
elif $CU_RENDERERS
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=OFF -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=ON
	make -j4
elif $CU_SAMPLES
then
	cmake -DCMAKE_BUILD_TYPE=Release ../source -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF -DCASTOR_BUILDGRP_TEST=OFF -DCASTOR_BUILDGRP_GENERATOR=OFF -DCASTOR_BUILDGRP_PARTICLES=OFF -DCASTOR_BUILDGRP_INTEROP=OFF -DCASTOR_BUILDGRP_SAMPLE=ON -DCASTOR_BUILDGRP_DIVIDER=OFF -DCASTOR_BUILDGRP_TONEMAPS=OFF -DCASTOR_BUILDGRP_GENERIC=OFF -DCASTOR_BUILDGRP_IMPORTER=OFF -DCASTOR_BUILDGRP_POSTFX=OFF -DCASTOR_BUILDGRP_RENDERER=OFF -DCASTOR_BUILD_SAMPLE_CASTOR_TD=OFF -DCASTOR_BUILD_SAMPLE_CASTOR_VIEWER=OFF -DCASTOR_BUILDGRP_RENDERER=OFF
	make -j4
fi
