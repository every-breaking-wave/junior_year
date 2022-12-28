package com.wave.backend.utils;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.type.MapType;
import com.fasterxml.jackson.databind.type.SimpleType;


import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class JsonUtil {

    private static final ObjectMapper MAPPER = new ObjectMapper();

    public static String objectToString(Object object){
        try {
            return MAPPER.writeValueAsString(object);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }
        return "{}";
    }

    public static <T> T stringToObject(String str, Class<T> clz){
        try {
            return MAPPER.readValue(str, clz);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static <K, V> Map<K, V> stringToMap(String str, Class<K> keyType, Class<V> valueType){
        try {
            return MAPPER.readValue(str, MapType.construct(HashMap.class, null, null, null, SimpleType.constructUnsafe(keyType), SimpleType.constructUnsafe(valueType)));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
