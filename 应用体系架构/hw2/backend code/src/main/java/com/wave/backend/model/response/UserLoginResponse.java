package com.wave.backend.model.response;

import com.wave.backend.constant.UserServiceStatus;
import lombok.Data;

import java.io.Serializable;

@Data
public class UserLoginResponse implements Serializable {

    private UserServiceStatus status;

    private String userAccount;

    private Integer id;


    private int role;

}
