ifeq ($(ARCH), x86_64)
CFLAGS += -mno-red-zone -mcmodel=kernel
endif

CFLAGS += -DRK_MODULE_NAME=\"$(NAME)\"

CFLAGS += -c -Iinclude -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC
CFLAGS += -Ibuild/bakey/include
ASFLAGS += -c -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC

C_SOURCES=$(filter %.c,$(SOURCES))
S_SOURCES=$(filter %.s,$(SOURCES))

C_OBJECTS=$(C_SOURCES:%.c=build/obj/$(SRCDIR)/%.o)
S_OBJECTS=$(S_SOURCES:%.s=build/obj/$(SRCDIR)/%.o)

OUTPUT=build/root/usr/lib/rk/$(SRCDIR).ko

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(C_OBJECTS) $(S_OBJECTS)
	@echo " LD     $@"
	@mkdir -pv $(dir $@)
	@$(LD) -r -o $@ $(C_OBJECTS) $(S_OBJECTS) $(LDFLAGS)

$(C_OBJECTS): build/obj/%.o: %.c
	@echo " CC     $<"
	@mkdir -pv $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $< -MD -MP -MF $(@:%.o=%.d)

$(S_OBJECTS): build/obj/%.o: %.c
	@echo " AS     $<"
	@mkdir -pv $(dir $@)
	@$(CC) $(ASFLAGS) -o $@ $< -MD -MP -MF $(@:%.o=%.d)

-include $(C_OBJECTS:%.o=%.d)
-include $(S_OBJECTS:%.o=%.d)
