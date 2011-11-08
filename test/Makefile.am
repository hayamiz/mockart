
TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES = \
	test-mockart.la

LDFLAGS += -module -rpath $(libdir) -avoid-version -no-undefined $(GLIB_LIBS)

INCLUDES =					\
	$(CUTTER_CFLAGS)			\
	$(GCUTTER_CFLAGS)			\
	$(GLIB_CFLAGS)				\
	-I$(top_srcdir)/src

LIBS += $(CUTTER_LIBS) $(GCUTTER_LIBS)

LIBADD = $(top_builddir)/src/libmockart.la

test_mockart_la_SOURCES = test-mockart.c
test_mockart_la_LIBADD = $(LIBADD)
test_mockart_la_CFLAGS = -O0

echo-cutter:
	@echo $(CUTTER)
