package io.github.vitarecomps.frontend;
public final class ControlLayout {
 private final ControlElement[] elements; private int count;
 public ControlLayout(int capacity){if(capacity<1)throw new IllegalArgumentException();elements=new ControlElement[capacity];}
 public int register(ControlElement element){for(int i=0;i<count;i++)if(elements[i].id.equals(element.id)){if(elements[i].type!=element.type)throw new IllegalArgumentException("ID used by another control type");elements[i]=element;return i;}if(count==elements.length)throw new IllegalStateException("Control capacity reached");elements[count]=element;return count++;}
 public ControlElement get(int i){if(i<0||i>=count)throw new IndexOutOfBoundsException();return elements[i];}
 public int size(){return count;}
}