---

layout: post
title: "Attacking the OAuth Protocol"
author: "Dhaval Kapil"
excerpt: "Analyzing the weaknesses of the OAuth 2.0 protocol"
keywords: oauth, oauth2.0, weakness, attack, exploit, security, csrf, xss, vulnerability

---

This post is about developing a secure OAuth 2.0 server, the inherent weaknesses of the protocol, and their mitigation.

> ## Introduction

Recently, I had the opportunity to mentor a fellow student at [SDSLabs](https://sdslabs.co/) on a project related to the OAuth 2.0 protocol. It was then that I decided to read the official [manual](https://tools.ietf.org/html/rfc6749) for OAuth 2.0. It took me a few hours to go through the entire document and analyze it.

The OAuth 2.0 protocol itself is **insecure**. The document specifies some security measures that are **optional** (which boils down to **missing** for the casual developer). Apart from that, there are additional loopholes as well. Herein, I try to enumerate the various vulnerabilities of the OAuth 2.0 protocol which I found after reading the standard and a couple of online resources. I suggest mitigation to each of these which might be either following the standard strictly or even changing the standard slightly.

This is aimed to benefit both: developers working with OAuth 2.0 as well as security researchers.

> ## Overview

I'll be assuming that the reader is familiar with the OAuth 2.0 protocol. There are tons of online [resources](https://aaronparecki.com/oauth-2-simplified/) to read up on this. The reader should also be familiar with basic attacks like [CSRF](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_(CSRF)), [XSS](https://www.owasp.org/index.php/Cross-site_Scripting_(XSS)) and [open redirect](http://whatis.techtarget.com/definition/open-redirect). I'll be mainly focussing on the _Authorization code grant_ and a little on the _Implicit grant_. As a refresher, these are the steps involved in an _Authorization code grant_:

1. The user requests the client to start the authorization process through the user-agent by issuing a GET request. This happens when the user clicks on 'Connect'/'Sign in with' button on the client's website.

2. The client redirects the user-agent to the authorization server using the following query parameters:
  * **response_type**: _code_
  * **client_id**: The _id_ issued to the client.
  * **redirect_uri**(optional): The URI where the authorization server will redirect the response to.
  * **scope**(optional): The scope to be requested.
  * **state**(recommended): An opaque value to maintain state between the request and callback.

3. After the user authenticates and grants authorization for requested resources, the authorization server redirects the user-agent to the _redirect_uri_ with the following query parameters:
  * **code**: The authorization code.
  * **state**: The value passed in the above request.

4. The client further uses the _authorization code_ to request for an access token(with appropriate client authentication) using the following parameters in the request body:
  * **grant_type**: _authorization\_code_
  * **code**: The authorization code received earlier.
  * **redirect_uri**: The _redirect\_uri_ passed in the first request.

> ## Attacks

Now, I'm going to talk about various attacks possible by modifying the above-mentioned requests. I'll be specifying the assumptions in each of the cases separately.

> ### Attacking the 'Connect' request

This attack exploits the first request mentioned above, i.e. the request generated when a user clicks 'Connect' or 'Sign in with' button. Many websites allow users to connect additional accounts like Google, Facebook, Twitter, etc. using OAuth. An attacker can gain access to the victim's account on the Client by connecting one of his/her own account(on the Provider).

**Steps**:

1. The attacker creates a dummy account with some Provider.

2. The attacker initiates the 'Connect' process with the Client using the dummy account on the Provider, but, stops the redirect mentioned in request 3(in the Authorization code grant flow). i.e. The attacker has granted _Client_ access to his/her resources on the _Provider_ but the _Client_ has not yet been notified.

3. The attacker creates a malicious webpage simulating the following steps:
  * Logging out the user on Provider(using CSRF).
  * Logging in the user on Provider with the credentials of his/her dummy account(using CSRF).
  * Spoofing the 1st request to connect the _Provider_ account with _Client_. This can be easily done, as it is just another _GET_ request. It is preferred to do this within an iframe so that the victim is unaware of this.

4. When the victim visits the attacker's page, he/she is logged out of _Provider_ and then gets signed in as the _dummy account_. The 'Connect' request is then issued which results in the attacker's dummy account to be connected with the victim's account on _Client_. Note that the victim will not be asked for granting access to the client as the attacker has already approved it in Step 2.

5. Now, the attacker can log in to the victim's account on _Client_ by signing in with the dummy account on _Provider_.

**Mitigation**

Although the vulnerability exists on the Provider itself(allowing CSRF log in and log out), it is even better to protect the 'Connect' page from allowing requests that do not originate from the user. This can be ensured by using a _csrf_token_ within the client to protect the 1st request. The OAuth 2.0 standard should specify this.

> ### Attacking 'redirect_uri'

Presently, to prevent attackers using arbitrary _redirect\_uri_, many OAuth servers partially match this parameter with a _redirect\_uri_ prespecified during client registration. Generally, during registration, the client specifies the domain and only those _redirect\_uri_ on that particular domain are allowed. This becomes dangerous when an attacker is able to find a page vulnerable, to say XSS, on the client's domain. The attacker can subsequently steal _authorization\_code_.

**Steps**:

1. The attacker is able to leak data(say through XSS) from a page on the client's domain: `https://client.com/vuln`.

2. The attacker injects Javascript code(if XSS) on that page that sends the URL loaded in the browser(with parameters as well as fragments) to the attacker.

3. The attacker creates a webpage that forces the user to visit a malicious link such as:
  `https://provider.com/oauth/authorize?client_id=CLIENT_ID&response_type=code&redirect_uri=https%3A%2F%2Fclient.com%2Fvuln`

4. When the victim loads this link, the user-agent is redirected to `https://client.com/vuln?code=CODE`. This `CODE` is then sent to the attacker.

5. The attacker can use this code at his/her end to issue an access token by passing it to the authentic _redirect\_uri_ such as `https://client.com/oauth/callback?code=CODE`.

This attack is even more dangerous if the authorization server supports the _Implicit grant_. By passing `response_type=token`, the attacker can steal the token directly.

**Mitigation**

To prevent the attack for _Authorization code grant_, OAuth already specifies the following in the standard for an access token request:

> The authorization server MUST:
>
> * ensure that the "redirect_uri" parameter is present if the "redirect_uri" parameter was included in the initial authorization request as described in Section 4.1.1, and if included ensure that their values are identical.

With this, the attacker will be unable to perform Step 5. The client will request for an access token with  _authentication\_code_ and authentic _redirect\_uri_ which will not match with `https://client.com/vuln`. Hence, the authorization server will not grant an access token. However, developers rarely take this into consideration. Individually, this does not represent any real threat, but with other vulnerabilities(as mentioned above), this can lead to leaking of _access tokens_. Note that, this will not prevent attacking authorization servers using _Implicit grant_.

Another protective measure, which in my opinion is more _secure_ and handles both the above cases is that the authorization server should **whitelist** a list of _redirect\_uri_. Also, while sanitizing this parameter, **exact** matches should be made instead of partial matches. Usually, clients have predefined _redirect\_uri_ and they rarely need to change them.

> ### CSRF on Authorization response

By performing a Cross Site Request Forgery attack, an attacker can link a dummy account on _Provider_ with victim's account on _Client_(as mentioned in the first attack). This attack uses the 3rd request of the Authorization code grant.

**Steps**:

1. The attacker creates a dummy account on _Provider_.

2. The attacker initiates the 'Connect' process with the Client using the dummy account on the Provider, but, stops the redirect mentioned in request 3(in the Authorization code grant flow). i.e. The attacker has granted _Client_ access to his/her resources on the _Provider_ but the _Client_ has not yet been notified. The attacker saves the _authorization\_code_.

3. The attacker forces the victim to make a request to: `https://client.com/<provider>/login?code=AUTH_CODE`. This can be easily done by making the victim opening a malicious webpage with any _img_ or _script_ tag with the above URL as _src_.

4. If the victim is logged in _Client_, the attacker's dummy account is now connected to his/her account.

5. Now, the attacker can log in to the victim's account on _Client_ by signing in with the dummy account on _Provider_.

**Mitigation**

OAuth 2.0 provides security against such attacks through the `state` parameter passed in the 2nd and 3rd request. It acts like a CSRF token. The attacker cannot forge a malicious URL without knowing the `state` which is user session specific. However, in the current implementation of OAuth, this parameter is **NOT** required and is optional. Developers not well versed with security are susceptible to ignore this.

OAuth 2.0 should force clients to send a `state` parameter and handle requests that are missing this parameter as 'error requests'. Proper guidelines should also be given for generating and handling csrf tokens.

_Note: Using the `state` parameter does not prevent the first attack mentioned above(Attacking the 'Connect' request)._

> ### Reusing an access token - One access_token to rule them all

OAuth 2.0 considers _access\_token_ to be independent of any client. All it ensures is that an _access\_token_ stored on the authorization server is mapped to appropriate _scopes_ and _expiration time_. An access token generated for _client1_ can be used for _client2_ as well. This poses a danger to clients using the _Implicit grant_.

**Steps**:

1. The attacker creates an authentic client application _client1_ and registers it with a _Provider_.

2. The attacker somehow manages to get the victim use _client1_. Thereby, he/she has access to the _access token_ of the victim on _client1_.

3. Assume that the victim uses _client2_ which further uses the _Implicit grant_. In _Implicit grant_, the authorization server redirects the user-agent to a URL such as: `https://client2.com/callback#access_token=ACCESS_TOKEN`. The attacker visits this URL with the _access\_token_ of the client.

4. _client2_ authenticates the attacker as the victim. Hence, a single access token can be used on many different clients that use _Implicit grant_.

**Mitigation**

Clients must ensure that the access token being used was indeed issued by them. Some OAuth server like Facebook, provide endpoints to get the __ a particular _access\_token_ was issued to: `https://graph.facebook.com/app?fields=id&access_token=ACCESS_TOKEN`.

> ### Open Redirect in OAuth 2.0

The OAuth 2.0 standard specifies the following guidelines for handling errors in Authorization requests:

> If the request fails due to a missing, invalid, or mismatching redirection URI, or if the client identifier is missing or invalid, the authorization server SHOULD inform the resource owner of the error and MUST NOT automatically redirect the user-agent to the invalid redirection URI.
>
> If the resource owner denies the access request or if the request fails for reasons other than a missing or invalid redirection URI, the authorization server informs the client by adding the following parameters to the query component of the redirection URI using the "application/x-www-form-urlencoded" format, per Appendix B:

Some OAuth servers, misinterpret this and interchange the order of the two checks. That is, if the request fails for reasons other than _redirection URI_, such as invalid scope, the server informs the client by redirecting it to the URL passed by the client without validating it. This makes the OAuth server to serve as an open redirector. A possible URL crafted by the attacker can be `https://provider.com/oauth/authorize?response_type=code&client_id=CLIENT_ID&scope=INVALID_SCOPE&redirect_uri=http://attacker.com/`.

This vulnerability was once [present](http://blog.intothesymmetry.com/2015/04/open-redirect-in-rfc6749-aka-oauth-20.html) in Facebook, Microsoft, and Google.

**Mitigation**

The mitigation is trivial: the authorization server should first validate the _redirect\_uri_ parameter and continue accordingly.

> ## Conclusion

In short, while developing an OAuth server, security should be kept in mind. Knowledge about various attack vectors is necessary. The OAuth specification _should_ be updated to enforce the appropriate security measures mentioned above. [Oauth by Sakurity](https://sakurity.com/oauth) is a great improvement over OAuth 2.0.

This list is not complete. If you know of any other attacks or even better ways to mitigate the above-mentioned attacks feel free to comment!
