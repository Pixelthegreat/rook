CFLAGS += -c -Iinclude
ASFLAGS += -c

C_SOURCES=$(filter %.c,$(SOURCES))
S_SOURCES=$(filter %.s,$(SOURCES))

C_OBJECTS=$(C_SOURCES:%.c=build/obj/$(SRCDIR)/%.o)
S_OBJECTS=$(S_SOURCES:%.s=build/obj/$(SRCDIR)/%.o)

.PHONY: all pre clean

all: $(OUTPUT)

$(OUTPUT): $(C_OBJECTS) $(S_OBJECTS)
	@echo " LD     $@"
	@mkdir -pv $(dir $@)
	@$(CC) -o $@ $(C_OBJECTS) $(S_OBJECTS) $(LDFLAGS)

$(C_OBJECTS): build/obj/%.o: %.c
	@echo " CC     $<"
	@mkdir -pv $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $< -MD -MP -MF $(@:%.o=%.d)

$(S_OBJECTS): build/obj/%.o: %.s
	@echo " AS     $<"
	@mkdir -pv $(dir $@)
	@$(CC) $(ASFLAGS) -o $@ $< -MD -MP -MF $(@:%.o=%.d)

-include $(C_OBJECTS:%.o=%.d)
-include $(S_OBJECTS:%.o=%.d)
