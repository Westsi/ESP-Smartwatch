package com.xonize.xswatchconnect.utils;

public class ByteConversions {
    public static int toint8(byte b) {return b & 0xFF;}

    public static byte[] byteArrayOfInts(int... ints) {
        byte[] bytes = new byte[ints.length];
        for (int i=0;i<ints.length;i++) {
            bytes[i] = (byte) ints[i];
        }
        return bytes;
    }
}
