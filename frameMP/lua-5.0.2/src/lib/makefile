PRJDIR=../../../${BLD_ROOT}
PRJNAME=$(LUA_PRJ_NAME)

LUADIR=../..

include $(PRJDIR)/config.mk
include $(PRJDIR)/crules.mk
include $(LUADIR)/lua.mk

OBJ_CORE:=$(patsubst ../%.c,$(OBJOUT)/$(PRJNAME)/%.o,$(wildcard ../*.c))

link:: $(LUA_LIB)

$(LUA_LIB): $(OBJ)
	$(AR) $@ $(OBJ) $(OBJ_CORE)

$(OBJ): $(OBJ_CORE)

all:: $(LUA_LIB)
