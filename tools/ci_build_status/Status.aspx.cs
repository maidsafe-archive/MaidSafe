using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Configuration;
using System.Text;
using System.Web.Configuration;

public partial class Status : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {
      try {
        Configuration root_config = WebConfigurationManager.OpenWebConfiguration(null);

        try {
          KeyValueConfigurationElement win7_64 = root_config.AppSettings.Settings["win7,x64"];
          if(win7_64.Value == "ok") {
            this.win7_64_status.Text = "Good";
            this.win7_64_status.CssClass = "green";
          } else {
            this.win7_64_status.Text = "Failure";
            this.win7_64_status.CssClass = "red";
            this.win7_64_project.Text = win7_64.Value.Split(',')[0];
            this.win7_64_project.CssClass = "red";
            this.win7_64_commiter.Text = win7_64.Value.Split(',')[1];
            this.win7_64_commiter.CssClass = "red";
          }
        } catch (Exception /*ex*/) { }

        try {
          KeyValueConfigurationElement win7_86 = root_config.AppSettings.Settings["win7,x86"];
          if(win7_86.Value == "ok") {
            this.win7_86_status.Text = "Good";
            this.win7_86_status.CssClass = "green";
          } else {
            this.win7_86_status.Text = "Failure";
            this.win7_86_status.CssClass = "red";
            this.win7_86_project.Text = win7_86.Value.Split(',')[0];
            this.win7_86_project.CssClass = "red";
            this.win7_86_commiter.Text = win7_86.Value.Split(',')[1];
            this.win7_86_commiter.CssClass = "red";
          }
        } catch (Exception /*ex*/) { }

        try {
          KeyValueConfigurationElement linux_64 = root_config.AppSettings.Settings["linux,x64"];
          if(linux_64.Value == "ok") {
            this.linux_64_status.Text = "Good";
            this.linux_64_status.CssClass = "green";
          } else {
            this.linux_64_status.Text = "Failure";
            this.linux_64_status.CssClass = "red";
            this.linux_64_project.Text = linux_64.Value.Split(',')[0];
            this.linux_64_project.CssClass = "red";
            this.linux_64_commiter.Text = linux_64.Value.Split(',')[1];
            this.linux_64_commiter.CssClass = "red";
          }
        } catch (Exception /*ex*/) { }

        KeyValueConfigurationElement linux_86 = root_config.AppSettings.Settings["linux,x86"];
        if(linux_86.Value == "ok") {
          this.linux_86_status.Text = "Good";
          this.linux_86_status.CssClass = "green";
        } else {
          this.linux_86_status.Text = "Failure";
          this.linux_86_status.CssClass = "red";
          this.linux_86_project.Text = linux_86.Value.Split(',')[0];
          this.linux_86_project.CssClass = "red";
          this.linux_86_commiter.Text = linux_86.Value.Split(',')[1];
          this.linux_86_commiter.CssClass = "red";
        }
      } catch (Exception /*ex*/) {
        return;
      }
    }
}