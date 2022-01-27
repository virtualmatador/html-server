Module['onload'] = [];
Module['onRuntimeInitialized'] = function()
{
    Module['onload'].forEach(task => { task(); });
    Module['onload'] = null;
};

Module['lazyLoad'] = function(widget, ...params)
{
    if (!document.getElementById(widget + '.css'))
    {
        const link = document.createElement('link');
        link.id = widget + '.css';
        link.rel = 'stylesheet';
        link.href = '/' + widget + '.css';
        document.head.appendChild(link);
    }
    if (!document.getElementById(widget + '.js'))
    {
        const script = document.createElement('script');
        script.id = widget + '.js';
        script.src = '/' + widget + '.js';
        document.head.appendChild(script);
    }
    const task = function ()
    {
        const template = document.createElement('template');
        template.innerHTML = Module[widget + '_render'].apply(null, params);
        while (template.content.childNodes.length > 0)
        {
            this.parentElement.insertBefore(
                template.content.childNodes[0], this);
        }
        this.parentElement.removeChild(this);
    }.bind(document.currentScript);
    if (Module['onload'] == null)
    {
        task();
    }
    else
    {
        Module['onload'].push(task);
    }
};
