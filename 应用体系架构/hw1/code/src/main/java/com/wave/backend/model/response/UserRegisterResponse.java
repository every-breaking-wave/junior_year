package com.wave.backend.model.response;

import com.wave.backend.constant.UserServiceStatus;
import lombok.Data;

import java.io.Serializable;

@Data
public class UserRegisterResponse implements Serializable {

    private UserServiceStatus status;

    private String userAccount;

    private Long id;

}
