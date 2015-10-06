---

layout: post
title: "Elasticsearch Lua"
author: "Dhaval Kapil"
excerpt: "Development of elasticsearch-lua, a client for elasticsearch written in Lua."
keywords: elasticsearch, lua, client, development, framework, REST, json, gsoc, google summer of code, open source

---

This post is about [elasticsearch-lua](https://github.com/DhavalKapil/elasticsearch-lua). I developed it during the summer of 2015 as part of [GSoC(Google Summer of Code) 2015](https://www.google-melange.com/gsoc/homepage/google/gsoc2015). Here I shall describe the architecture of the client and the reasons for various software design decisions that I took.

**Note: This post is not about 'How to use elasticsearch-lua'. I would recommend you to go through the [README](https://github.com/DhavalKapil/elasticsearch-lua) and the [documentation](https://dhavalkapil.com/elasticsearch-lua/docs/classes/Client.html) on how to use elasticsearch-lua.**

> ## Introduction

Elasticsearch is a very powerful and scalable search engine. It provides a REST API accessed through JSON format. There are many clients written in different languages (e.g. php, js, python, ruby) that wrap around the REST API to provide an abstraction. However, there was no client for Lua. My project aimed to create an elasticsearch client for Lua developers.

As part of GSOC-2015 I worked with the [LabLua](http://www.lua.inf.puc-rio.br/) "organization", a research laboratory at PUC-Rio dedicated to research about programming languages, especially Lua, and my mentor [Pablo Musa](http://www.inf.puc-rio.br/~pmusa).

> ## Motivation

I develop applications under the student group [SDSLabs](http://sdslabs.co/). One of our applications had a text-search feature on a large dataset. Using `Elasticsearch` in the backend, greatly enhanced its performance. Seeing its potential, I was greatly interested in it.

Later when I read about the `elasticsearch-lua` project under GSoC I was quite excited. I was getting the opportunity to develop a client for `elasticsearch` in `Lua`. Clearly it had a lot of uses for the developers using the Lua language. My interest in software networking also motivated me to work on this project.

> ## Developing elasticsearch-lua

### Influenced by other official clients

Much of the client was influenced by the official clients, mainly [elasticsearch-php](https://github.com/elastic/elasticsearch-php), [elasticsearch-js](https://github.com/elastic/elasticsearch-js), and  [elasticsearch-py](https://github.com/elastic/elasticsearch-py). Before even starting with the design of the client I went through the architecture of these clients to get acquainted with the feature set they provide and the conventions they follow. My reason for doing so was to basically prevent 'reinventing the wheel' while stable and widely used clients were already present. Also providing the same kind of conventions would help any user, who has already used these other clients, to feel at home while working with the client. This is evident from the below example in which both codes look quite similar. The same applies to most functions as well.

> #### Using the Javascript client to get a document

      var elasticsearch = require('elasticsearch');

      var client = new elasticsearch.Client({
        host: 'localhost:9200',
        log: 'trace'
      });

      client.get({
        index: 'my_index',
        type: 'my_type',
        id: 'my_dox'
      }, function (error, response) {
        // ...
      });

> #### Using the Lua client to get a document

      local elasticsearch = require "elasticsearch"

      local client = elasticsearch.client{
        hosts = {
          { host = "localhost",
            port = 9200
          }
        }
      }

      local data, err = client:get{
        index = "my_index",
        type = "my_type",
        id = "my_doc"
      }

### Using Object Oriented Programming

Although Lua does not provide any special construct for declaring objects and classes, I preferred going with object oriented approach mostly because I was used to it. **Lua has only the concept of tables.** To create an object model over the Lua table, I followed the [approach suggested by the language author](http://www.lua.org/pil/16.html).

### Logging

I looked into [log4j](http://logging.apache.org/log4j/2.x/) while including logging in the client. Log levels defined in the client are same as compared with log levels in log4j. The user can specify the log levels while creating the client object.

### Lua specific constructs

Instead of throwing errors using `error()` function, I kept on with the Lua convention of returning `nil, error`. This enables the user to systematically handle errors as and when required. Also, there is a conceptual difference between 'Application Error' and 'Request Error'.

Lua has the concept of passing parameters by reference. Hence, sometimes I maintained a copy of variables to work on instead of directly changing the passed parameters.

### Parameter checking while requesting endpoints

Every request to an endpoint in elasticsearch is associated with various parameters that are passed in the HTTP request. Every kind of endpoint has a list of allowed parameters. I discussed with my mentor regarding the possibility of checking user parameters before sending a request. In the end I decided to to implement parameter checking because of the following reasons:

1. The user should focus more on his application rather than elasticsearch.
2. The user should be notified of invalid parameters as it saves time in debugging his/her own code.
3. Not much overhead is required to check the parameters passed by the user.
4. Sending invalid requests will only put unnecessary load on the server and be time-consuming for the client.

Overall it’s been a wonderful experience developing elasticsearch-lua and working for Google Summer of Code. It was a great learning for me. I shall continue developing it further. Feel free to contribute!