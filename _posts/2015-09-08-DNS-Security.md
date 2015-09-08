---

layout: post
title: "DNS Security"
author: "Dhaval Kapil"
excerpt: "Different types of attacks concerning DNS and their mitigation"
keywords: DNS, protocol, domain, attack, security, hack, poison, spoof, zone, compromise, footprinting, DOS, flooding, mitm, hijacking

---

> ## Introduction

The Domain Name System is an essential component of the functionality of most Internet services. It provides a distributed solution for services such as resolving host names to IP addresses and vice versa. DNS was designed around the early 1980s without any security consideration. This was mainly because at that time networks were quite small. All the hosts in the network were known beforehand and trustworthy. There was no need for authenticity. 

But as the network grew and Internet was born DNS remained unchanged. This resulted in lots of threats that target DNS due to the lack of authenticity and integrity checking of data held within the DNS. In 1994, the Internet Engineering Task Force (IETF) started working to add security extensions known as Domain Name System Security Extensions (DNSSEC) to the existing DNS protocol. Unfortunately, these extensions are still far from being adopted.

A discussion on each of these topics is presented in this blog.

> ## About DNS

As per [wikipedia](https://en.wikipedia.org/wiki/Domain_Name_System)

> The Domain Name System (DNS) is a hierarchical distributed naming system for computers, services, or any resource connected to the Internet or a private network. It associates various information with domain names assigned to each of the participating entities. Most prominently, it translates domain names, which can be easily memorized by humans, to the numerical IP addresses needed for the purpose of computer services and devices worldwide.

![DNS about](/assets/images/DNS-Security/dns_about.png)

_Image Source: [here](http://www.codeguru.com/cpp/sample_chapter/article.php/c12013/Sample-Chapter-Domain-Name-System.htm)_

DNS is like a phone book for the Internet. One can't remember each and every IP for the websites that he or she visits. It's much easier to remember the domain names instead.

> ## Threats involving DNS

1. Zone File Compromise
2. Zone Information Leakage/DNS Footprinting
3. DNS Amplification Attack
4. DNS Client flooding
5. DNS Cache poisoning
6. DNS Vulnerabilities in Shared Host Environments
7. DNS Man in the Middle Attacks - DNS Hijacking
8. Typosquatting

> ### Zone File Compromise

The DNS server is hosted on a number of machines. The administrator can configure the DNS server including the DNS records using either command line interface or a GUI interface provided by the DNS server.

In a Zone File Compromise attack, the attacker attacks the DNS server by gaining direct access to these machines. He/she can be in physical contact with the server or connected through an SSH/RDP connection.

**Security measures**: Restrict access to the DNS server, both physically and remotely.

> ### Zone Information Leakage/DNS Footprinting

_DNS Zone Transfer_ involves a DNS server passing a copy of part of its database(called "zone") to another DNS server. Zone transfer is used when we need to have more than one DNS server answering queries for a particular zone. There is a Master DNS Server and some Slave DNS Servers. A Slave DNS Server asks for zone transfer from the Master DNS Server.

The attacker just pretends to be a Slave DNS Server and asks the Master DNS Server for a copy of the records. These records reveal a lot about the topology of the internal network.
Steps for performing Zone Transfer(on a UNIX machine):

      dig NS 'domain' +short

Displays the authoritative name servers for that domain

      dig AXFR 'domain' @'nameserver'

Retrieves all the DNS records from a particular nameserver

**Security measures**: Restrict zone transfers to particular IP addresses or use any other kind of authentication.

> ### DNS Amplification Attack

This type of attack is used to perform DOS attack on a victim host using genuine DNS servers.
It involves sending DNS packets to a DNS server, spoofed with source IP as the victim IP. The DNS server responds back with much larger DNS responses that go to the victim host.

> ### DNS Client Flooding

DNS Client Flooding aims at sending a flood of UDP requests to the DNS server to exhaust its resources. A common technique is to send DNS request packets for an invalid domain. The DNS server spends its resources to look for this domain. After a certain limit, it has no resources to serve legitimate requests.

> ### DNS Cache poisoning

A client queries its configured DNS server for resolving a domain name. This DNS server queries other DNS servers for that domain name and after getting a result, caches it, till the corresponding TTL value. Until this TTL value, client queries for that particular domain name are retrieved from the cache instead of making further queries to other DNS servers for that domain.

This can be abused by an attacker to place false information in the local DNS server's cache. The attacker needs to reply back to the configured DNS server with the malicious address before the actual reply comes back.

![DNS cache poisonoing](/assets/images/DNS-Security/dns_cache_poisoning.png)

_Image Source: [here](https://jfdm.host.cs.st-andrews.ac.uk/notes/netsec/)_

> ### DNS Vulnerabilities in Shared Host Environments

A shared host environment is where one DNS server is shared amongst many users and domains. 

Free Services such as `cpanel` provide such facilities.

Say an attacker using a shared DNS server creates a zone file for `xyz.com` domain and adds relevant `A` and `MX` records. Now any user who has the said DNS server configured as primary from a client will when attempting to go to `xyz.com` be directed to the records as configured by the client i.e. potentially false information.

![DNS shared host vulnerability](/assets/images/DNS-Security/dns_shared_host_vulnerability.png)

_Image Source: [here](http://www.net-security.org/dl/articles/Attacking_the_DNS_Protocol.pdf)_

> ### DNS Man in the Middle Attacks - DNS Hijacking

An attacker can intercept the name resolution queries sent by the client to a DNS server. He/she can send back incorrect replies back to the client. This type of attack is very much a race condition, in that the attacker needs to get his reply back to the client before the legitimate server does. The client shall only look at the first response it gets and there is no way it can differentiate between the attacker or its DNS server.

![DNS Man in the Middle attack](/assets/images/DNS-Security/dns_mitm.png)

_Image Source: [here](http://www.net-security.org/dl/articles/Attacking_the_DNS_Protocol.pdf)_

> ### Typosquatting

Definition from [wikipedia](https://en.wikipedia.org/wiki/Typosquatting):

> Typosquatting, also called URL hijacking, sting site, or fake URL, is a form of cybersquatting, and possibly brandjacking which relies on mistakes such as typographical errors made by Internet users when inputting a website address into a web browser. Should a user accidentally enter an incorrect website address, they may be led to any URL (including an alternative website owned by a cybersquatter).

The attacker registers similar sounding domain names. This threat does not target a particular victim.

> ## DNSSEC (Domain Name System Security Extensions)

Around 1994, the IETF started a discussion to make DNS secure by adding a set of extensions to it. These extensions, labeled as Domain Name System Security Extensions (DNSSEC), were formally published in 2005. Unfortunately, even after around 10 years, DNSSEC is still not adopted even though its backward compatible. Mostly because network operatives prefer stability over complexity.

Backward compatibility was enforced by using the RR (Resource Record) construct of the DNS that was purposely designed to be extensible. A new set of RRs was defined that holds the security information. While designing DNSSEC, performance issues were kept in mind.

In all DNSSEC provides authentication and integrity to the DNS. This helps in preventing many attacks. Cache poisoning and Client flooding attacks are prevented with the addition of source authentication. Even Zone File Compromise attack is mitigated. Note however that DNSSEC does not provide any security against information leakage.

**Note**: I recently did a talk about DNS Security in my college. Slides can be found [here](https://speakerdeck.com/dhavalkapil/dns-security).