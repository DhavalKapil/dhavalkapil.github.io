---

layout: post
title: "Elasticsearch Lua (II)"
author: "Dhaval Kapil"
excerpt: "Google Summer of Code (GSoC) 2016 report"
keywords: elasticsearch, lua, client, development, framework, REST, json, gsoc, google summer of code, open source

---

This post is about my [GSOC](https://summerofcode.withgoogle.com/projects/#5987313834262528) project, that I worked on during summer, 2016. I worked on adding a test suite and documentation for [elasticsearch-lua](https://github.com/DhavalKapil/elasticsearch-lua).

> ## Introduction

[Elasticsearch](https://www.elastic.co/products/elasticsearch) is a distributed, scalable and full-text search engine based on Lucene. It provides an HTTP web interface and handles JSON documents. It is presently [ranked 1](http://db-engines.com/en/ranking/search+engine) in the category of 'Search engines'.

[elasticsearch-lua](https://github.com/DhavalKapil/elasticsearch-lua) is a client for Elasticsearch that provides a wrapper over the REST interface for the Lua Programming Language. I developed it last as part of [GSOC 2015](https://www.google-melange.com/archive/gsoc/2015/orgs/lablua/projects/dhavalkapil.html) with my mentor [Pablo Musa](http://www.inf.puc-rio.br/~pmusa).

My GSOC project this year was entitled 'Improve elasticsearch-lua tests and builds' and was a continuation of the work that I had done last year. Apart from adding a test suite for `elasticsearch-lua` and making it robust, I also decided to work on the documentation of the code.

> ## Test suite for elasticsearch-lua

The tests are divided into unit, integration and stress tests. Note that all these tests run for Lua 5.1, 5.2, 5.3 and LuaJIT 2.0. Code coverage is measured for unit tests and integration tests. [Coveralls](https://coveralls.io/github/DhavalKapil/elasticsearch-lua) was chosen to measure and maintain code coverage. As of now, around 91% of the code is covered with tests.

> ### Unit Tests

There are many different modules within `elasticsearch-lua`. For every such module, there is a corresponding unit test written. Unit tests can be found in `tests/` directory. Care was taken to test extensively all the endpoints. Some key points to note:

* Some modules were '[mocked](http://stackoverflow.com/questions/2665812/what-is-mocking)' to intercept external calls.

* Not only return values (success or failure) but every internal parameter was 'deep' checked. Deep check involves checking each nested parameter recursively. For example, a lua table might have another table inside it.

* [Travis](https://travis-ci.org/DhavalKapil/elasticsearch-lua) was chosen for continuous integrations. Everytime code is pushed, a build is triggered on travis and unit tests are run. Success or failure status is reported back.

* A number of bugs(pertaining to url forming and rockspec file) were found by running the tests. All were fixed.

The diff of changes due to unit tests can be seen [here](https://github.com/dhavalkapil/elasticsearch-lua/compare/5bab5d0a73ecee82db6f64584a0cc38176e3d216...dhavalkapil:61220c1625253e8d72a3e2e1cc108e7b5db01af4).

> ### Integration Tests

Apart from the test of every component individually, it is equally important that they work together while interacting with each other. To make `elasticsearch-lua` robust, it was necessary to add some integration tests.

* Integration tests involve calling an API function in a real environment and testing parameters at every point. [Wrappers](https://github.com/DhavalKapil/elasticsearch-lua/blob/master/tests/lib/operations.lua) for some API functions were developed so as to avoid repeated code.

* Elasticsearch is a search engine. There was a need for a 'big' dataset. Data available freely from [www.githubarchive.org](https://www.githubarchive.org/) was chosen to be used. A mirror is maintained [here](https://dhavalkapil.com/elasticsearch-test-dataset/dataset/2015-01-01-15.json.gz). The dataset is not a part of the main repository due to  size, so it is downloaded on the fly while running tests on travis.

* Common operations (such as search, index, get, delete and bulk) were tested in a single run. These operations are intermixed together.

The diff of changes due to integration tests can be seen [here](https://github.com/dhavalkapil/elasticsearch-lua/compare/8216f944d45e5b1da2710716b6b9eb980330f432...dhavalkapil:822990f5878094fad2db5e3519b96036b6afb32b).

> ### Stress Tests

Stress tests involve rigorous testing of elasticsearch-lua. By having these tests, the client will be able to prove its stability in an effective manner.

* A separate framework for stress testing was designed, considering that it might take a few hours to finish. In short, every successful (unit + integration tests) build triggers a new build, which runs the stress tests, provided that no such build is already running.

* The status of stress tests is reported through a separate badge in the [README](https://github.com/dhavalkapil/elasticsearch-lua).

The diff of changes due to stress tests can be seen [here](https://github.com/dhavalkapil/elasticsearch-lua/compare/768e4bee6e26f7f5ca902b21314b9d60947d7873...dhavalkapil:e59f095896c43f8f419bc1847c53d47c3824fa12).

> ## Documentation

Having a good documentation is very important for any library. It helps developers to understand functionalities without having to investigate the code. Moreover, it helps the library adoption as new developers can use it as a guide to get started. For this very purpose, I opted to invest  a lot of time in the documentation. However, initially, this was not a task for the GSOC project. Realizing its importance, I decided to complete it and added it to my timeline.

> ### Guides

The guides consist of documents and tutorials that help developers to install, use and customize elasticsearch-lua. The guides explain the most frequently used functionalities along with some internals. These pages are hosted [here](http://elasticsearch-lua.readthedocs.io/en/latest/).

> ### API Documentation

The API Documentation lists all possible functions provided by the elasticsearch-lua. Each function name is accompanied by the parameters that it accepts. The API documentation is published [here](https://dhavalkapil.com/elasticsearch-lua/docs/).

The diff of changes pertaining to documentation can be seen [here](https://github.com/dhavalkapil/elasticsearch-lua/compare/04d33706890a64c4247ad73e1430fad6f8b8d681...dhavalkapil:9fa3d1a7cde5fa357b6c54e69cae2a0ba6c8d0d9).

> ## Additional tasks(Not part of GSOC)

Apart from the tasks mentioned above, I worked on the following as well:

> ### Luaver

While working with `elasticsearch-lua`, I had to frequently switch between different versions of lua while developing the test suite. Switching is not simple and I faced the following issues often:

1. Building different lua versions required some effort such as downloading the version source, unzipping, installing and managing any dependency faced. Also, the previous version had to be deleted completely in order to avoid any ambiguity.

2. Luarocks installation depends on the Lua version. Switching lua versions can mess up the installed rocks.

3. To solve these issues I used workaround methods, such as editing the source code of some existing rocks.

4. Sometimes, these code changes broke the entire rock. In such cases, I had to remove all existing rocks, rebuild luarocks and then reinstall the needed rocks.

Also, I was already familiar with NodeJS and Ruby and understood how such problems were addressed by [nvm](https://github.com/creationix/nvm) and [rvm](https://github.com/rvm/rvm).

This motivated me to create a similar tool for lua, and that is how [luaver](https://dhavalkapil.com/luaver) was born. I wrote a separate [blog](/blogs/Lua-Version-Manager) post about luaver. Help [donate](https://gratipay.com/luaver/) to luaver. Initially, I didn't expect to spend much time on it and figured that I could manage both GSoC and develop luaver simultaneously. However, at some point in time, I got too involved in luaver which resulted in me getting one week behind the timeline that I had proposed for GSoC. Nevertheless, I covered it up soon.

> ### Updating elasticsearch-lua

It is important that the client implements all the features provided by Elasticsearch. Also, Elasticsearch is evolving a lot releasing in a fast pace, so it is important that clients are also up-to-date. Some features, though not much important, were missing and the client version was 1.6 while Elasticsearch is in 2.3. Therefore, I decided to update the library and add missing features as well. Presently, I’m still working on this part.

> ## Benefits of working on the same project for two consecutive years

1. I myself had written the client. The codebase was already at my finger-tips. I could spend more time working than understanding and getting comfortable with the code.

2. I wanted to further consolidate my client and make it stable. I couldn't get much time during the rest of the year to work full-fledged on the development. Google Summer of Code offered a nice incentive.

3. I had already worked with the Lua community. Being in familiar environment, I was able to work and think freely. [luaver](https://dhavalkapil.com/luaver) was created to benefit the open source Lua community. If this was my first time I wouldn't even have thought about developing it.

