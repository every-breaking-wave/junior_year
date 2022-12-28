package org.reins.one;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

@CrossOrigin(maxAge = 3600)
@RestController
public class GreetingController {
	@Autowired
	private HttpSession hs;

	@GetMapping("/about")
	public String getAbout() {
		return "This is a Spring security sample";
	}

	@GetMapping("/users")
	public String getUser() {
		return "I am a user";
	}

	@GetMapping("/")
	public String getHome() {
		if (hs.getAttribute("token") == null){
			hs.setAttribute("token", "New");
			System.out.println("token is null");
		}
		else {
			System.out.println("token is NOT null");
			hs.setAttribute("token", "Old");
		}
		return "Let' start! Server Two: " + hs.getAttribute("token") + " " + hs.getId();
	}

}
