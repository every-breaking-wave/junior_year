package com.wave.backend;

import com.github.dreamyoung.mprelation.AutoMapper;
import com.wave.backend.entity.BookLabel;
import com.wave.backend.mapper.OrderMapper;
import com.wave.backend.entity.Order;
import com.wave.backend.repository.BookLabelRepository;
import com.wave.backend.service.OrderService;
import lombok.extern.slf4j.Slf4j;
import org.junit.jupiter.api.Test;
import org.mybatis.spring.annotation.MapperScan;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.web.servlet.ServletComponentScan;
import org.springframework.data.neo4j.repository.config.EnableNeo4jRepositories;

import javax.annotation.Resource;

@SpringBootTest
@Slf4j
@EnableNeo4jRepositories
@MapperScan("com.wave.backend.mapper")
@ServletComponentScan
class BackendApplicationTests {

    @Resource
    private OrderService orderService;
    @Resource
    private OrderMapper orderMapper;
    @Resource
    private AutoMapper autoMapper;

    @Resource
    private BookLabelRepository bookLabelRepository;

    @Test
    void test() {
        Order order = orderMapper.selectById(177414145);
        autoMapper.mapperEntity(order);
        log.info(order.toString());
        System.out.println(order);
        log.info(order.getOrderItems().toString());
    }

    @Test
    void testNeo4j(){
        BookLabel bookLabel = new BookLabel();
        bookLabel.setLabelName("科幻");
        bookLabelRepository.save(bookLabel);
    }

}
