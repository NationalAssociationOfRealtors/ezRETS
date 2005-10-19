CFLAGS		+= $(TARGET_CFLAGS) -D_DEBUG

FIXDEP		= perl $(srcdir)/project/build/fixdep.pl
ALL_DEPENDS 	= $(COMMON_DEPENDS) $(DRIVER_DEPENDS) $(EXAMPLES_DEPENDS) # $(SETUP_DEPENDS)
ALL_OBJ_DIRS	= \
	build/common/objects \
	build/common/lib \
	build/driver/objects \
	build/driver/lib \
	build/examples/objects \
	build/examples/bin

ALL		= $(COMMON_LIB) $(DRIVER_LIB) $(EXAMPLES_EXE) # $(SETUP_LIB)

_all: _build

_debug:
	@echo $(USE_DEPENDS)

prepare:
	@mkdir -p $(ALL_OBJ_DIRS) $(EXE_DIR) $(DOC_DIR)

_build: prepare $(ALL)

_doc: prepare $(DOC_FILES_TO_GEN)

_doc-api: prepare
	$(RM) -r build/doc/api
	doxygen project/build/Doxyfile

# (Ab)use rsync as a recursive copy with exclude.  Yeah, rsync is
# "non-standard", but screw it.  It's unfortunately the easiest way to
# do a recursive copy with exclude, especially with the cool
# --cvs-exclude option.  Must manually copy "./configure" over since
# it is ignored in .cvsignore.
_dist: _build _doc-api
	$(RM) -r dist/odbcrets
	mkdir -p dist/odbcrets
	mkdir -p dist/odbcrets/doc
	rsync -a --cvs-exclude project/driver/include dist/odbcrets
	rsync -a --cvs-exclude build/doc/api dist/odbcrets/doc
	cd dist; zip -r -q odbcrets.zip odbcrets
	cd dist; tar --gzip -cf odbcrets.tar.gz odbcrets

_clean:
	$(RM) -r build dist

_distclean: _clean
	$(RM) $(DISTCLEAN_FILES)
	$(RM) -r autom4te.cache
	$(RM) makefile

_veryclean: _distclean

_test: prepare $(DRIVER_TEST_EXE)
	./$(DRIVER_TEST_EXE)

_maintainer-clean: _veryclean
	$(RM) configure

build/driver/odbcrets/objects/%.d: project/driver/src/%.ccp
	@echo Generating dependencies for $<
	@mkdir -p $(dir $@)
	@$(CC) -MM $(CFLAGS) $< | $(FIXDEP) > $@

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@echo Generating dependencies for $<
	@mkdir -p $(dir $@)
	@$(CC) -MM $(CFLAGS) $< | $(FIXDEP) > $@

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cc
	@echo Generating dependencies for $<
	@mkdir -p $(dir $@)
	@$(CXX) -MM $(CFLAGS) $< | $(FIXDEP) > $@

ifeq ($(USE_DEPENDS),1)
-include $(ALL_DEPENDS)
endif

.PHONY: all debug build doc doc-api \
	insatll install-bin install-data install-config no-cppunit \
	_all _debug _build _doc _doc-api _clean _distclean _veryclean \
	_maintainer-clean test _test
