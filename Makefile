UNAME_S := $(shell uname -s)
CC := gcc
DOXYGEN := doxygen
CFLAGS := -Iinclude -Iinclude/mu -c -g -Wall -Wextra -Werror -Wno-int-in-bool-context -Wno-misleading-indentation -Wno-shift-negative-value -Wno-attributes -DMUPARSER_STATIC
CPPFLAGS := -std=c++11
ifeq ($(UNAME_S), Linux)
	LDFLAGS := -lstdc++ -lm -lglfw
endif
ifeq ($(findstring MSYS, $(UNAME_S)), MSYS)
	LDFLAGS := -Llib -lglfw3dll -lgdi32 -lstdc++
endif
DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJDIR)/$*.d
OUTDIR := bin
DLLDIR := deploy
EXEC_NAME := $(OUTDIR)/GraphAnalyze
DOXYGEN_CONFIG := GraphAnalyzeDoc
RESDIR := res
SRCDIR := src
OBJDIR := obj
SOURCES := $(wildcard $(SRCDIR)/*.c* $(SRCDIR)/*/*.c*)
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o, $(SOURCES))
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o, $(OBJS))

.PHONY: all clean docs run $(RESDIR)

all: $(OBJDIR) $(OBJDIR) $(OUTDIR) $(EXEC_NAME)
	@cp -r $(DLLDIR)/* $(OUTDIR)
	@[ "$(shell ls -A $(RESDIR))" ] && cp -r $(RESDIR)/* $(OUTDIR) || :

clean:
	rm -f $(EXEC_NAME)
	rm -rf $(OUTDIR)
	rm -rf $(OBJDIR)
	rm -rf $(OBJDIR)

run: all
	@echo ">>> Running $(EXEC_NAME) ..."
	@$(EXEC_NAME)

docs:
	@$(DOXYGEN) $(DOXYGEN_CONFIG)

$(OUTDIR):
	@mkdir $(OUTDIR)

$(OBJDIR):
	@mkdir $(OBJDIR)

$(EXEC_NAME): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

-include $(patsubst $(OBJDIR)/%.o,$(OBJDIR)/%.d,$(OBJS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEPFLAGS) $< -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) $< -o $@
