package io.github.vitarecomps.frontend;
public final class ControlElement {
 public enum Type { BUTTON, MENU, DPAD, LEFT_ANALOG, RIGHT_ANALOG }
 public final String id; public final Type type; public final int priority,commandMask;
 public ControlElement(String id,Type type,int priority,int commandMask){if(id==null||!id.matches("[A-Za-z0-9._-]{1,80}")||type==null)throw new IllegalArgumentException("Invalid control ID/type");this.id=id;this.type=type;this.priority=priority;this.commandMask=commandMask;}
}