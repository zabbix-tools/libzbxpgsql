# -*- mode: ruby -*-
# vi: set ft=ruby :
VAGRANTFILE_API_VERSION = "2"

$script_common = <<script
BULLET="==>"

script

$script_rhel7 = <<script

script

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = "chef/centos-7.0"

  config.vm.provision "shell", inline: $script_common
  
  # Primary CentOS 7.0 4 bit box
  # vagrant up && vagrant ssh
  config.vm.define "centos7", primary: true do |default|
    # Forward port for phpPgAdmin
    default.vm.network "forwarded_port", guest: 80, host: 8080
    
    # Pre-configuration
    default.vm.provision "shell", path: "vagrant/setup_centos7.sh"
  end

  # Ubuntu 12.04 LTS 64 bit box
  # vagrant up precise64 && vagrant ssh precise64
  config.vm.define "precise64", autostart: false do |p64|
    p64.vm.box = "hashicorp/precise64"
    p64.vm.provision "shell", path: "vagrant/setup_ubuntu12.sh"
  end
end
