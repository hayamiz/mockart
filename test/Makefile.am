
TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES =				\
	mocking-target.la			\
	test-mockart.la				\
	test-steal-strdup.la

LDFLAGS += -module -rpath $(libdir) -avoid-version -no-undefined $(GLIB_LIBS)

INCLUDES =					\
	$(CUTTER_CFLAGS)			\
	$(GCUTTER_CFLAGS)			\
	$(GLIB_CFLAGS)				\
	-I$(top_srcdir)/src \
	-fno-builtin

LIBS += $(CUTTER_LIBS) $(GCUTTER_LIBS)

LIBADD = $(top_builddir)/src/libmockart.la

mocking_target_la_SOURCES = mocking-target.c

test_mockart_la_SOURCES = test-mockart.c
test_mockart_la_LIBADD = $(LIBADD)

test_steal_strdup_la_SOURCES = test-steal-strdup.c
test_steal_strdup_la_CFLAGS = -fno-builtin -O0
test_steal_strdup_la_LIBADD = $(LIBADD) mocking-target.la

echo-cutter:
	@echo $(CUTTER)
