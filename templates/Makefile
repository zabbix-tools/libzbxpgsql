# update older templates automatically, from the latest template

LATEST = Template_PostgreSQL_Server_3.0.xml

all: \
	Template_PostgreSQL_Server_2.0.xml \
	Template_PostgreSQL_Server_2.4.xml

Template_PostgreSQL_Server_2.0.xml: $(LATEST)
	zabbix-template-convertor \
		$(LATEST) -o 2.0.0 \
		> Template_PostgreSQL_Server_2.0.xml

Template_PostgreSQL_Server_2.4.xml: $(LATEST)
	zabbix-template-convertor \
		$(LATEST) -o 2.4.0 \
		> Template_PostgreSQL_Server_2.4.xml

clean:
	rm -vf \
		Template_PostgreSQL_Server_2.0.xml \
		Template_PostgreSQL_Server_2.4.xml

.PHONY: all clean
