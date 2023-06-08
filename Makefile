#FIXME Use CPPFLAGS, LDLIBS

LIBYOCTO = yocto
LIBYOCTO_MAJOR = 0
LIBYOCTO_MINOR = 1.0

LIBYOCTO_NAME = lib${LIBYOCTO}.so
LIBYOCTO_SONAME = ${LIBYOCTO_NAME}.$(LIBYOCTO_MAJOR)
LIBYOCTO_FULLNAME = ${LIBYOCTO_SONAME}.$(LIBYOCTO_MINOR)

TARGET_LIBYOCTO = lib${LIBYOCTO}
TARGET_BOARDTEST = boardtest
TARGET_SETLED = setled

MACHINES = AMD64 RBOX630 GWS501 RADIPV3 IFB
ifneq ($(filter $(MACHINES), $(MACHINE)), )
    $(info Building for machine: $(MACHINE))
	CFLAGS += -D$(MACHINE)
else
    $(error Missing MACHINE=[$(MACHINES)])
endif

SRC_DIR = src
BUILD_DIR = build/$(MACHINE)
OBJ_DIR = $(BUILD_DIR)/.obj
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib

CFLAGS_COMMON = $(CFLAGS) -Wall -Wextra -pedantic -O2 -g -MMD -MP
CFLAGS_LIBYOCTO = $(CFLAGS_COMMON) -fPIC `pkg-config --cflags glib-2.0` -I $(SRC_DIR)/$(TARGET_LIBYOCTO) -I $(SRC_DIR)/$(TARGET_LIBYOCTO)/rbox

LDFLAGS_LIBYOCTO = $(LDFLAGS) -shared -Wl,-soname,${LIBYOCTO_SONAME} -lpthread -lsystemd `pkg-config --libs glib-2.0`
ifeq ($(MACHINE), RBOX630)
	LDFLAGS_LIBYOCTO += -li2c -lgpiod
endif
ifeq ($(MACHINE), RADIPV3)
	LDFLAGS_LIBYOCTO += -lgpiod
endif

CFLAGS_BOARDTEST = $(CFLAGS_COMMON) -I $(SRC_DIR)/$(TARGET_LIBYOCTO)
LDFLAGS_BOARDTEST = $(LDFLAGS) -L$(LIB_DIR) -l$(LIBYOCTO)

CFLAGS_SETLED = $(CFLAGS_BOARDTEST)
LDFLAGS_SETLED = $(LDFLAGS_BOARDTEST)

RM = rm -f
LN = ln -fsr

SRCS = $(sort $(shell find $(SRC_DIR) -name '*.c'))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:$(OBJ_DIR)/%.o=$(OBJ_DIR)/%.d)

getSources = $(SRC_DIR)/$(1)/%.c
getObjects = $(filter $(OBJ_DIR)/$(1)/%.o , $(OBJS))

.PHONY: all
all: $(TARGET_LIBYOCTO) $(TARGET_BOARDTEST) $(TARGET_SETLED)

.PHONY: test
test: ${TARGET_BOARDTEST}
	LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/$(TARGET_BOARDTEST)

.PHONY: valgrind
valgrind: ${TARGET_BOARDTEST}
#glib still reachable: 21,583 bytes in 113 blocks
#   18,612 bytes in 6 blocks: link
#   21,583 bytes in 113 blocks: with configNetwork
	LD_LIBRARY_PATH=$(LIB_DIR) G_DEBUG=gc-friendly G_SLICE=always-malloc valgrind -s --suppressions=/usr/share/glib-2.0/valgrind/glib.supp --show-leak-kinds=all --leak-resolution=high --num-callers=20 $(BIN_DIR)/$(TARGET_BOARDTEST)

.PHONY: default
default: all

.PHONY: $(TARGET_LIBYOCTO)
$(TARGET_LIBYOCTO): $(LIB_DIR)/${LIBYOCTO_NAME}

.PHONY: $(TARGET_BOARDTEST)
$(TARGET_BOARDTEST): $(BIN_DIR)/${TARGET_BOARDTEST}

.PHONY: $(TARGET_SETLED)
$(TARGET_SETLED): $(TARGET_LIBYOCTO) $(BIN_DIR)/${TARGET_SETLED}

#Symbolic link
$(LIB_DIR)/${LIBYOCTO_NAME}: $(LIB_DIR)/${LIBYOCTO_SONAME}
	$(LN) $< $@

$(LIB_DIR)/${LIBYOCTO_SONAME}: $(LIB_DIR)/${LIBYOCTO_FULLNAME}
	$(LN) $< $@

#Link
$(LIB_DIR)/${LIBYOCTO_FULLNAME}: $(call getObjects,$(TARGET_LIBYOCTO))
	@mkdir -p $(@D)
	$(CC) $^ $(LDFLAGS_LIBYOCTO) -o $@

$(BIN_DIR)/$(TARGET_BOARDTEST): $(call getObjects,$(TARGET_BOARDTEST)) $(LIB_DIR)/${LIBYOCTO_NAME}
	@mkdir -p $(@D)
	$(CC) $^ $(LDFLAGS_BOARDTEST) -o $@

$(BIN_DIR)/$(TARGET_SETLED): $(call getObjects,$(TARGET_SETLED)) $(LIB_DIR)/${LIBYOCTO_NAME}
	@mkdir -p $(@D)
	$(CC) $^ $(LDFLAGS_SETLED) -o $@

#Compile
$(OBJ_DIR)/$(TARGET_LIBYOCTO)/%.o: $(call getSources,$(TARGET_LIBYOCTO))
	@mkdir -p $(@D)
	$(CC) ${CFLAGS_LIBYOCTO} -c $< -o $@

$(OBJ_DIR)/$(TARGET_BOARDTEST)/%.o: $(call getSources,$(TARGET_BOARDTEST))
	@mkdir -p $(@D)
	$(CC) ${CFLAGS_BOARDTEST} -c $< -o $@

$(OBJ_DIR)/$(TARGET_SETLED)/%.o: $(call getSources,$(TARGET_SETLED))
	@mkdir -p $(@D)
	$(CC) ${CFLAGS_SETLED} -c $< -o $@

-include $(DEPS)

.PHONY: install
install:
	install -d $(prefix)/$(includedir)
	install -D -m 0755 $(SRC_DIR)/$(TARGET_LIBYOCTO)/$(TARGET_LIBYOCTO).h $(prefix)/$(includedir)

	install -d $(prefix)/$(libdir)
	cp -d $(LIB_DIR)/* $(prefix)/$(libdir)
	chmod 0755 $(prefix)/$(libdir)/*

	install -d $(prefix)/$(bindir)
	install -D -m 0755 $(BIN_DIR)/$(TARGET_SETLED) $(prefix)/$(bindir)
	install -D -m 0755 $(BIN_DIR)/$(TARGET_BOARDTEST) $(prefix)/$(bindir)

.PHONY: clean
clean:
	-${RM} -r $(BUILD_DIR)
