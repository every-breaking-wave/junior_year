package com.wave.backend.constant;

public interface UserConstant {
    /**
     *  用户登录态键
     */
    String USER_LOGIN_STATE = "userLoginState";

    String USER_ID = "userId";

    /**
     * 权限： 默认权限：0， 管理员权限： 1
     */
    int DEFAULT_ROLE = 0;
    int ADMIN_ROLE = 1;

    int NOT_LOGIN = 3;

}
