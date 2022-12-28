package com.wave.backend.controller;

import com.wave.backend.model.KafkaMessage;
import com.wave.backend.model.response.CreateOrderResponse;
import com.wave.backend.service.OrderService;
import com.wave.backend.utils.InfoMessageEncoder;
import com.wave.backend.utils.MessageDecoder;
import lombok.Data;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.configurationprocessor.json.JSONException;
import org.springframework.boot.configurationprocessor.json.JSONObject;
import org.springframework.kafka.annotation.KafkaListener;
import org.springframework.kafka.core.KafkaTemplate;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import javax.sound.sampled.Line;
import javax.websocket.DecodeException;
import javax.websocket.EncodeException;
import java.util.HashMap;
import java.util.Map;

@Component
@Data
public class OrderListener {

    @Autowired
    private OrderService orderService;

    @Autowired
    private KafkaTemplate<String, String> kafkaTemplate;

    @Autowired
    private WebSocketServer ws;

    @Resource
    private MessageDecoder decoder;


    @Resource
    private InfoMessageEncoder encoder;

    @KafkaListener(topics = "topic1", groupId = "id_test")
    public void topicListener(ConsumerRecord<String, String> record) throws DecodeException, EncodeException {
        String userId =  record.value();
        CreateOrderResponse createOrderResponse =  orderService.createOrder(Integer.valueOf(userId));
        KafkaMessage kafkaMessage = new KafkaMessage(createOrderResponse.getStatus().toString(), createOrderResponse.getPrice());
        ws.sendMessageToUser(createOrderResponse.getUserId().toString(), encoder.encode(kafkaMessage) );
        kafkaTemplate.send("topic2", "key",  encoder.encode(kafkaMessage));
    }

    @KafkaListener(topics = "topic2", groupId = "id_test")
    public void topic2Listener(ConsumerRecord<String, String> record) {
        String value = record.value();
        System.out.println("topic2 get message, create_order_response : " + value);
    }

}
