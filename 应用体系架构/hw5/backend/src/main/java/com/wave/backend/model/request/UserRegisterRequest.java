package com.wave.backend.model.request;

import lombok.Data;

import java.io.Serializable;


/**
 * 用户注册请求体
 */

// 继承 Serializable ,使类序列化
@Data
public class UserRegisterRequest implements Serializable {

    private static final long serialVersionUID = -543286974259314538L;
    private String userAccount;

    private String userPassword;

    private String repeatPassword;

    private String email;

}
