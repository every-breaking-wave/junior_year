package com.wave.backend.utils;

import com.wave.backend.model.KafkaMessage;
import org.apache.kafka.common.serialization.Serializer;

import java.nio.ByteBuffer;
import java.util.Map;

public class UserDefineSerializer implements Serializer<KafkaMessage> {
    private final String encoder = "UTF-8";
 
    @Override
    public void configure(Map configs, boolean isKey) {
 
    }
 
    @Override
    public byte[] serialize(String topic, KafkaMessage kafkaMessage) {
        if (kafkaMessage == null) return null;
        byte[]  status;
 
        try {
            if (kafkaMessage.getStatus() != null) {
                status = kafkaMessage.getStatus().getBytes(encoder);
            } else {
                status = new byte[0];
            }
//            ByteBuffer buffer = ByteBuffer.allocate(4 + 4 + 4 + status.length);
//            buffer.putInt(kafkaMessage.getId());
//            buffer.putInt(kafkaMessage.getUserId());
//            buffer.putInt(status.length);
//            buffer.put(status);
//            return buffer.array();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new byte[0];
    }
 
    @Override
    public void close() {
 
    }
}