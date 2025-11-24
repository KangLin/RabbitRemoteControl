// Author: Kang Lin <kl222@126.com>
// Auto fill user name and password

(function() {
    var gPasswordManager;

    function hostFromLocation() {
        try { return window.location.hostname; } catch(e){ return ""; }
    }

    function fill() {
        var host = hostFromLocation();
        if (!host) {
            console.error("AutoFill: Host is empty");
            return;
        }

        if (!gPasswordManager) {
            console.error('AutoFill: the password manager is null');
            return;
        }

        // 从 C++ 获取凭据并尝试填写
        try {
            // 连接信号。因为调用 C++ 函数是异步返回。
            gPasswordManager.sigCredentialsReady.connect(function(host, creds) {
                if (host !== hostFromLocation()) return;
                //console.log("AutoFill: get host: " + host + "; username: " + creds.username + "; creds: " + creds);
                // creds 是普通 JS 对象：creds.username / creds.password
                if (creds && creds.username) {
                    // 尝试找到常见的用户名/密码 input
                    var usernameInput = document.querySelector('input[type="email"], input[type="text"], input[name*="user"], input[name*="login"], input[id*="user"], input[id*="login"]');
                    var passwordInput = document.querySelector('input[type="password"]');
                    if (usernameInput && creds.username) {
                        usernameInput.focus();
                        usernameInput.value = creds.username;
                        usernameInput.dispatchEvent(new Event('input', { bubbles: true }));
                        usernameInput.dispatchEvent(new Event('change', { bubbles: true }));
                    }
                    if (passwordInput && creds.password) {
                        passwordInput.focus();
                        passwordInput.value = creds.password;
                        passwordInput.dispatchEvent(new Event('input', { bubbles: true }));
                        passwordInput.dispatchEvent(new Event('change', { bubbles: true }));
                    }
                }
            });
            var creds = gPasswordManager.getCredentials(host);
        } catch(e) {
            // getCredentials 可能抛异常（或返回空），忽略
            console.log("AutoFill: getCredentials error", e);
        }
    }

    // 监听 form 提交，尝试保存凭据
    function onSubmit(ev) {
        try {
            var host = hostFromLocation();
            if (!host) {
                console.error("AutoFill: Host is empty");
                return;
            }
            //console.log("AutoFill: host:" + host);
            var form = ev.target;
            var username = '';
            var password = '';
            var u = form.querySelector('input[type="email"], input[type="text"], input[name*="user"], input[name*="login"], input[id*="user"], input[id*="login"]');
            var p = form.querySelector('input[type="password"]');
            if (u) username = u.value;
            if (p) password = p.value;
            if (username && password) {
                // 调用 C++ 保存
                gPasswordManager.saveCredentials(host, username, password);
            }
        } catch(e) {
            console.log("AutoFill: onSubmit error", e);
        }
    }

    // 为页面上所有 form 注册 submit 监听（包括动态添加的）
    function attachFormListeners() {
        var forms = document.querySelectorAll('form');
        forms.forEach(function(f) {
            if (!f.__autofillBound) {
                f.addEventListener('submit', onSubmit, true);
                f.__autofillBound = true;
            }
        });
    }

    function start(channel) {
        gPasswordManager = channel.objects.PasswordManager;
        if (!gPasswordManager) {
            console.error('AutoFill: the password manager is null');
            return;
        }
        fill();
        attachFormListeners();
        // 观察 DOM 变化，给新表单也绑定
        var observer = new MutationObserver(function() {
            attachFormListeners();
        });
        observer.observe(document, { childList: true, subtree: true });
    }

    function AutoFill() {
        if (typeof QWebChannel === 'undefined') {
            console.error('AutoFill: QWebChannel not loaded');
            return;
        }
        if (typeof qt === 'undefined') {
            console.error('AutoFill: qt not available');
            return;
        }
        if (!qt.webChannelTransport) {
            console.error('AutoFill: qt.webChannelTransport not available');
            return;
        }

        // 等待 QWebChannel 可用（qwebchannel.js 已被注入）
        new QWebChannel(qt.webChannelTransport, function(channel) {
            start(channel);
        });
    }

    //console.log("AutoFill: doumment.readyState:" + document.readyState);
    //如果 QWebChannel 已经可用，直接 AutoFill; 否则在 ready 事件后再试
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', AutoFill);
    } else {
        AutoFill();
    }

})();
