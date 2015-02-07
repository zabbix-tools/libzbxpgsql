# -*- mode: ruby -*-
# vi: set ft=ruby :
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # Primary CentOS 7.0 64 bit box
  # vagrant up centos7 && vagrant ssh centos7
  config.vm.define "centos7", primary: true do |rhel7|
    rhel7.vm.box = "chef/centos-7.0"
    rhel7.vm.provision "shell", path: "vagrant/setup_centos7.sh"
  
    # Forward port for phpPgAdmin
    rhel7.vm.network "forwarded_port", guest: 80, host: 8080
  end

  # CentOS 6.5 64 bit box
  # vagrant up centos7 && vagrant ssh centos7
  config.vm.define "centos6", autostart: false do |rhel6|
    rhel6.vm.box = "chef/centos-6.5"
    rhel6.vm.provision "shell", path: "vagrant/setup_centos6.sh"
  end

  # Ubuntu 12.04 LTS 64 bit box
  # vagrant up precise64 && vagrant ssh precise64
  config.vm.define "precise64", autostart: false do |p64|
    p64.vm.box = "hashicorp/precise64"
    p64.vm.provision "shell", path: "vagrant/setup_ubuntu12.sh"
  end
end
