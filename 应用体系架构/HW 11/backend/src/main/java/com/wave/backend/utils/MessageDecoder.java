/**
 * Copyright (c) 2014 Oracle and/or its affiliates. All rights reserved.
 *
 * You may not modify, use, reproduce, or distribute this software except in
 * compliance with  the terms of the License at:
 * https://github.com/javaee/tutorial-examples/LICENSE.txt
 */
package com.wave.backend.utils;

import com.wave.backend.model.KafkaMessage;
import org.springframework.stereotype.Component;

import javax.json.Json;
import javax.json.stream.JsonParser;
import javax.websocket.DecodeException;
import javax.websocket.Decoder;
import javax.websocket.EndpointConfig;
import java.io.StringReader;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/* Decode a JSON message into a JoinMessage or a ChatMessage.
 * For example, the incoming message
 * {"type":"chat","name":"Peter","target":"Duke","message":"How are you?"}
 * is decoded as (new ChatMessage("Peter", "Duke", "How are you?"))
 */

@Component
public class MessageDecoder implements Decoder.Text<KafkaMessage> {
    /* Stores the name-value pairs from a JSON message as a Map */
    private Map<String,String> messageMap;

    @Override
    public void init(EndpointConfig ec) { }
    
    @Override
    public void destroy() { }
    
    /* Create a new Message object if the message can be decoded */
    @Override
    public KafkaMessage decode(String string) throws DecodeException {
        KafkaMessage msg = null;
        if (willDecode(string)) {
            if ("info".equals(messageMap.get("type"))) {
                msg = new KafkaMessage(messageMap.get("status"),  Double.valueOf(messageMap.get("price")));
            }
        } else {
            throw new DecodeException(string, "[Message] Can't decode.");
        }
        return msg;
    }
    
    /* Decode a JSON message into a Map and check if it contains
     * all the required fields according to its type. */
    @Override
    public boolean willDecode(String string) {
        boolean decodes = false;
        /* Convert the message into a map */
        messageMap = new HashMap<>();
        JsonParser parser = Json.createParser(new StringReader(string));
        while (parser.hasNext()) {
            if (parser.next() == JsonParser.Event.KEY_NAME) {
                String key = parser.getString();
                parser.next();
                String value = parser.getString();
                messageMap.put(key, value);
            }
        }
        /* Check the kind of message and if all fields are included */
        Set keys = messageMap.keySet();
        if (keys.contains("type")) {
            switch (messageMap.get("type")) {
                case "join":
                    if (keys.contains("name"))
                        decodes = true;
                    break;
                case "chat":
                    String[] chatMsgKeys = {"name", "target", "message"};
                    if (keys.containsAll(Arrays.asList(chatMsgKeys)))
                        decodes = true;
                    break;
            }
        }
        return decodes;
    }
}
