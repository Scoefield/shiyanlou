---
show: step
version: 1.0
enable_checker: true
---

# Vue-Router 浅析原理及使用方法

## 实验介绍

单页面开发模式的路由全权交由前端来控制，这里的路由不是我们说的路由器等硬件设备，它是前端的路径管理器。而 Vue-Router 是官方提供的路由插件，市面上也不乏路由管理插件，但是只有 Vue-Router 能和 Vue 深度集成完美搭配。

#### 知识点

- 了解 Vue-Router
- 路由模式
- 路由原理分析
- 路由的使用

## Vue-Router 路由模式

#### 单页面与传统页面跳转的区别

- 单页面

Vue 的单页面开发模式是基于组件和路由的配合，所有的页面均可视为组件，路由控制着访问路径，而每个路径映射一个组件。在单页模式中使用 a 标签做路由跳转是行不通的，因为本质上单页模式就只有一个 index.html 页面，所有的页面组件都在打包好的 JS 文件中，所以要使用 Vue-Router 的路由组件来跳转，这里的原因在后文中会有具体的例子解析。

- 传统页面

传统页面的开发模式，路由一般是由超链接（a 标签）来控制页面的跳转与切换，每次都会刷新整个页面，体验上不如单页模式。

Vue-Router 为我们提供了三种路由模式：

- Hash 模式
- History 模式
- abstract 模式

#### Hash 模式

Hash 模式是 Vue-Router 的默认模式，具体的体现是在浏览器地址栏上 URL 路径永远带着一个「#」号。在浏览器支持度上面，Hash 模式是比较强势的，甚至能兼容低版本的 IE 浏览器。「#」号后面内容的改变，不会引起页面对服务端的请求，所以也就不会重新加载页面。在部署服务器方面，个人认为 Hash 模式比 History 模式来得更为方便，因为 History 有 URL 重定向问题，需要在服务端去配置 url 重定向，否则会报 404 错误。市面上也有不少大厂是使用 Hash 模式进行开发的，比如网易云音乐：

![](https://doc.shiyanlou.com/courses/2573/358858/84cbf985fecb57bed95c1997a4bdfd09-0/wm)

#### History 模式

HTML5 History API 提供了一个 history.pushState 和 history.reolaceState 方法（浏览器支持情况不是很乐观），它能让开发人员在不刷新网页的情况下改变站点的 URL。因为 Hash 模式会带上一个「#」号，会让 URL 地址变得比较难看，所以很多开发者都会选择 History 模式开发。但有个缺点，前端的 URL 必须和实际向服务端发起的请求的 URL 保持一致，如果服务端没有对相应的路由做处理，则会返回 404 错误页面。

#### abstract 模式

abstract 模式针对的是没有浏览器环境的情况，比如 Weex 客户端开发，内部是没有浏览器 API 的，那么 Vue-Router 自身会对环境做校验，强制切换到 abstract 模式，如果默认在 Vue-Router 的配置项中不写 mode 的值，在浏览器环境下会默认启用 Hash 模式，在移动客户端下使用 abstract 模式。

## 原理解析

#### Hash 模式原理

大家常常会在浏览器上看到这样一种场景，点击某个文字，网页会跳转到某一个固定的位置，并且页面不会刷新。这便是浏览器的 a 标签锚点。Hash 模式被运用在了单页面开发的路由模式上，下面我们来简单实现一个通过 Hash 去控制页面组件的展示。
浏览器原生方法为我们提供了一个监听事件 `hashchange`，它能监听到的改变如下：

- 点击 a 标签改变 URL 地址；
- 浏览器的前进后退行为；
- 通过 `window.location` 方法改变地址栏。

以上三种情况都会触发 `hashchange` 监听事件，通过这个事件我们可以获取到 `localtion.hash`，继而去匹配相应的组件，下面是简易代码实现：

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Hash 模式</title>
  </head>
  <body>
    <div>
      <ul>
        <li><a href="#/page1">page1</a></li>
        <li><a href="#/page2">page2</a></li>
      </ul>
      <div id="route-view"></div>
    </div>
    <script type="text/javascript">
      // 下面为hash的路由实现方式
      // 第一次加载的时候，不会执行hashchange监听事件，默认执行一次
      window.addEventListener("DOMContentLoaded", Load);
      window.addEventListener("hashchange", HashChange);
      var routeView = null;
      function Load() {
        routeView = document.getElementById("route-view");
        HashChange();
      }
      function HashChange() {
        console.log("location.hash", location.hash);
        switch (location.hash) {
          case "#/page1":
            routeView.innerHTML = "page1";
            return;
          case "#/page2":
            routeView.innerHTML = "page2";
            return;
          default:
            routeView.innerHTML = "page1";
            return;
        }
      }
    </script>
  </body>
</html>
```

当初始的 HTML 文档被完全加载和解析完成之后， `DOMContentLoaded` 事件被触发，而无需等待样式表、图像和子框架的完成加载。 `hashchange` 不会被默认触发，所以网页首次加载完成后，需要默认执行一次 `hashchange` 监听方法要执行的函数 `HashChange`，当我点击代码中的两个 a 标签时，URL 地址栏改变触发 `hashchange` 事件， `HashChange` 方法通过拿到 `location.hash` 去匹配相应的组件（这里假设 page1、page2 为页面容器组件）。

接下来我们利用实验楼环境通过 Koa 运行上面这段 Html 代码，首先我们进入实验环境，新建文件夹，运行如下命令：

```bash
mkdir router
```

然后进入文件夹，初始化一个项目：

```bash
cd router
npm init -y // 直接回车
```

项目就会多了一个 `package.json` 文件，此时我们便可以安装 npm 工具包了，运行如下代码安装 Koa：

```bash
cnpm i Koa -S
```

之后我们创建 app.js 文件，代码如下：

```javascript
// app.js
const Koa = require("koa");
const app = new Koa();
const index = require("./index");

app.use((ctx, next) => {
  ctx.type = "text/html,charset=utf-8";
  ctx.body = index;
});

app.listen(8080);
```

再创建 index.js 文件：

```javascript
// index.js
module.exports = `<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Hash 模式</title>
  </head>
  <body>
    <div>
      <ul>
        <li><a href="#/page1">page1</a></li>
        <li><a href="#/page2">page2</a></li>
      </ul>
      <div id="route-view"></div>
    </div>
    <script type="text/javascript">
      // 下面为hash的路由实现方式
      // 第一次加载的时候，不会执行hashchange监听事件，默认执行一次
      window.addEventListener("DOMContentLoaded", Load);
      window.addEventListener("hashchange", HashChange);
      var routeView = null;
      function Load() {
        routeView = document.getElementById("route-view");
        HashChange();
      }
      function HashChange() {
        console.log("location.hash", location.hash);
        switch (location.hash) {
          case "#/page1":
            routeView.innerHTML = "page1";
            return;
          case "#/page2":
            routeView.innerHTML = "page2";
            return;
          default:
            routeView.innerHTML = "page1";
            return;
        }
      }
    </script>
  </body>
</html>`;
```

回到命令行，执行如下命令：

```bash
node app.js
```

点击左边栏的「Web 服务」，我们来看看浏览器的表现效果：

![](https://doc.shiyanlou.com/courses/1244/358858/a5627b89beca781820b9e18a4ad722e6-0/wm)

这样我们就实现了一个简易版的 Hash 路由模式，大家可以自己手动实现一下，加深记忆。

#### History 模式原理

下面来介绍 History 模式。

通过 History 模式去控制路由，会遇到一些麻烦，根本原因是 History 模式利用的时候 `popstate` 监听事件无法监听到 `pushState`、 `replaceState`、a 标签这三种形式的变化，浏览器的前进后退是可以监听到。那么有什么好的解决方案吗？

> 小知识：pushState 和 replaceState 都是 HTML5 的新 API，他们的作用很大，可以做到改变浏览器地址却不刷新页面。

我们可以通过遍历页面上的所有 a 标签，阻止 a 标签的默认事件的同时，加上点击事件的回调函数，在回调函数内获取 a 标签的 href 属性值，再通过 `pushState` 去改变浏览器的 `location.pathname` 属性值。然后手动执行 `popstate` 事件的回调函数，去匹配相应的路由。逻辑上可能有些饶，我们用代码来解释一下：

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>History 模式</title>
  </head>
  <body>
    <div>
      <ul>
        <li><a href="/page1">page1</a></li>
        <li><a href="/page2">page2</a></li>
      </ul>
      <div id="route-view"></div>
    </div>
    <script type="text/javascript">
      // 下面为history的路由实现方式
      window.addEventListener("DOMContentLoaded", Load);
      window.addEventListener("popstate", PopChange);
      var routeView = null;
      function Load() {
        routeView = document.getElementById("route-view");
        PopChange();
        // 获取所有带 href 属性的 a 标签节点
        var aList = document.querySelectorAll("a[href]");
        // 遍历 a 标签节点数组，阻止默认事件，添加点击事件回调函数
        aList.forEach((aNode) =>
          aNode.addEventListener("click", function (e) {
            e.preventDefault(); //阻止a标签的默认事件
            var href = aNode.getAttribute("href");
            //  手动修改浏览器的地址栏
            history.pushState(null, "", href);
            // 通过 history.pushState 手动修改地址栏，
            // popstate 是监听不到地址栏的变化，所以此处需要手动执行回调函数 PopChange
            PopChange();
          })
        );
      }
      function PopChange() {
        console.log("location", location);
        switch (location.pathname) {
          case "/page1":
            routeView.innerHTML = "page1";
            return;
          case "/page2":
            routeView.innerHTML = "page2";
            return;
          default:
            routeView.innerHTML = "page1";
            return;
        }
      }
    </script>
  </body>
</html>
```

同样将上面代码复制到 index.js 文件下替换，效果如下所示：

![](https://doc.shiyanlou.com/courses/1244/358858/6e3206229aceed4535356e251e61f7c8-0/wm)

这就是我们上述提到的 History 模式下，会遇到的麻烦。想要解决这个问题需要配合项目的 Web 服务器如 Express、Koa 等配置服务端返回的内容。

## 路由使用

接下来我们采用 ES2015 的形式简单讲解 Vue Router 的使用。

#### 引入 Vue-Router

首先是 HTML，我们引入 Vue 和 Vue Router 的静态资源，大家可以去 [BootCDN](https://www.bootcdn.cn/) 搜索自己需要的静态资源，下面是 HTML 代码：

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <script src="https://cdn.bootcss.com/vue/2.6.11/vue.min.js"></script>
    <script src="https://cdn.bootcss.com/vue-router/3.1.3/vue-router.min.js"></script>
    <title>Vue Router</title>
  </head>
  <body>
    <div id="app">
      <ul>
        <!-- 使用 router-link 组件来导航. -->
        <!-- 通过传入 to 属性指定链接. -->
        <!-- <router-link> 默认会被渲染成一个 <a> 标签 -->
        <router-link to="/page1">Go to Page1</router-link>
        <br />
        <router-link to="/page2">Go to Page2</router-link>
      </ul>
      <!-- 路由出口 -->
      <!-- 路由匹配到的组件将渲染在这里 -->
      <router-view></router-view>
    </div>
    <script type="text/javascript">
      // 1. 定义 (路由) 组件。
      // 可以从其他文件 import 进来，这里简单写
      const Page1 = { template: "<div>Page1</div>" };
      const Page2 = { template: "<div>Page2</div>" };

      // 2. 定义路由
      // 每个路由应该映射一个组件。 其中"component" 可以是
      // 通过 Vue.extend() 创建的组件构造器，
      // 或者，只是一个组件配置对象。
      const routes = [
        { path: "/page1", component: Page1 },
        { path: "/page2", component: Page2 },
      ];

      // 3. 创建 router 实例，然后传 routes 配置
      // 你还可以传别的配置参数, 不过先这么简单着吧。
      const router = new VueRouter({
        routes: routes,
      });

      // 4. 创建和挂载根实例。
      // 记得要通过 router 配置参数注入路由，
      // 从而让整个应用都有路由功能
      const app = new Vue({
        router: router,
      }).$mount("#app");
    </script>
  </body>
</html>
```

`<router-view>` 标签和 `<router-link>` 标签是引入 Vue-Router 后才能使用的，路由匹配到的组件都会渲染在 `<router-view>` 标签上， `<router-link>` 标签用于路由的跳转。

Vue-Router 默认启动 Hash 模式，同样复制到 index.js，重启 `node app.js` 启动页面的时候，会看到如下所示：

![](https://doc.shiyanlou.com/courses/1244/358858/8b46a3f62d1168bc950bf454dab80d54-0/wm)

#### 获取全局路由跳转参数的变化

想要全局监听路由的变化，可以在入口页面通过 watch `$router`对象来实现，修改上述代码：

```html
<script>
  ...
   // 4. 创建和挂载根实例。
   // 记得要通过 router 配置参数注入路由，
   // 从而让整个应用都有路由功能。
   const app = new Vue({
     router: router,
     watch: {
       $route(to, from) {
         console.log('to', to)
         console.log('from', from)
       }
     }
   }).$mount('#app')
   ...
</script>
```

![](https://doc.shiyanlou.com/courses/1244/358858/ff49fc90f55d18fcef87f725b9897377-0/wm)

添加监听之后，可以拿到 to 和 from 两个参数，to 代表你跳转后的页面参数，from 代表你从哪个页面跳转来的，通过拿到这两个参数，我们就能设置一级二级页面，用于制作过场动画。也可以添加页面顶部的页面加载进度条。

#### 获取路由中带的参数

Page1 页面在跳转的时候带上参数到 Page2，我们如何在 Page2 拿到参数呢？首先我们要给 `router-link` 标签添加参数，如下所示：

```html
<router-link :to="{path: '/page1', query: { id: 1111 }}"
  >Go to Page1</router-link
>
```

在 Page1 的模板页面拿到参数：

```javascript
const Page1 = { template: "<div>Page1 {{ $route.query.id }}</div>" };
```

![](https://doc.shiyanlou.com/courses/1244/358858/8daac07225ec56a8d35075c004d7fed3-0/wm)

#### 重定向页面

这点还是比较重要的，当你输入一个不存在的路由，Vue-Router 无法匹配到的时候，需要默认回到首页，这就要用到重定向匹配，代码如下所示：

```javascript
const routes = [
  { path: "/page1", component: Page1 },
  { path: "/page2", component: Page2 },
  { path: "*", redirect: "/page1" },
];
```

这里我们默认 Page1 为首页，当 `*` 上面的路径都没有被匹配到的时候，就会重新回到 `/page1` 对应的组件。

![](https://doc.shiyanlou.com/courses/1244/358858/76567c361aba5c0b98d2d9372022d999-0/wm)

以上是 Vue-Router 的一些基础用法，更多深度使用请移步 [官方文档](https://router.vuejs.org/zh/)详细阅读。

## 实验总结

路由是单页应用必备工具，市面上单页面应用框架如 Vue、React、Angular 的路由配置都大同小异，触类旁通。掌握好 Vue-Router 意味着你去看 React 的路由 React-Router 也是轻易便能上手，所以也请同学们好好对待。
