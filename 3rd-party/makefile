# Makefile common to all 3rd party submodules

BUILD_DIR=$@/BUILD

CMOCKA_CMAKE_FLAGS=-DWITH_STATIC_LIB=ON
.PHONY: cmocka
cmocka:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake $(CMOCKA_CMAKE_FLAGS) ..
	make -C $(BUILD_DIR) cmocka_static

clean-%: %/.
	rm -rf $(@:clean-%=%)/BUILD
