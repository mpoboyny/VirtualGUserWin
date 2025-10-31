
PRJDIR=../${BLD_ROOT}
PRJNAME=$(MP_VER_NAME)

include $(PRJDIR)/cxxrules.mk

LUADIR=../${LUA_VER_DIR}

CXXINCLUDES+= -I$(LUADIR)/include
HDR+=$(LUADIR)/include/lua.hxx

FRAMEMP_HDR=$(wildcard ../include/$(MP_NAME)/*.hxx)

all:: $(MP_LIB) $(LUA_LIB)

$(OBJ): $(FRAMEMP_HDR)

$(MP_LIB): $(OBJ)
	$(AR) $@ $(OBJ) $(LUA_LIB)
