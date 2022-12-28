package com.wave.backend.utils;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.TimeUnit;

@Component
public class RedisUtil {

    @Resource
    private RedisTemplate<String, Object> redisTemplate;

    public void set(String key, Object value) {
        redisTemplate.opsForValue().set(key, value);
    }

    public Object get(String key) {
        return redisTemplate.opsForValue().get(key);
    }

    public void setWithExpire(String key, Object value, Long time, TimeUnit timeUnit) {
        redisTemplate.opsForValue().set(key, value, time, timeUnit);
    }

    public boolean hasKey(String key) {
        return Boolean.TRUE.equals(redisTemplate.hasKey(key));
    }

    public boolean del(String key) {
        return Boolean.TRUE.equals(redisTemplate.delete(key));
    }

    public void incrementLong(String key, Long increment) {
        redisTemplate.opsForValue().increment(increment.toString());
    }

    public Long add(String key, Collection<Object> values) {
        return redisTemplate.opsForSet().add(key, values);
    }

    public Boolean isMember(String key, Object value) {
        return redisTemplate.opsForSet().isMember(key, value);
    }

    public void put(String key, String field, Object value) {
        redisTemplate.opsForHash().put(key, field, value);
    }

    public void putAll(String key, Map<Object, Object> map) {
        redisTemplate.opsForHash().putAll(key, map);
    }

    public Object getMapValue(String key, String field) {
        return redisTemplate.opsForHash().get(key, field);
    }

    public void flush() {
        Set<String> keys = redisTemplate.keys("*");
        if (keys == null)
            return;
        for(String key : keys) {
            redisTemplate.delete(key);
        }
//        redisTemplate.delete(keys);
    }

    public Long decrement(String key) {
        return redisTemplate.opsForValue().decrement(key);
    }

    public Long increment(String key) {
        return redisTemplate.opsForValue().increment(key);
    }

}
