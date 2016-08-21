---
layout: page
title: Connection pooling
permalink: /documentation/connection-pooling/
---

Monitoring your PostgreSQL server will consume additional backend connections.
While these connections are shortlived, and typically sparse (with each item
check spread over the configured check intervals), it is good practice to
implement connection pooling. This will minimize memory and semaphore
identifiers consumed by connections for the monitoring agent.

Rather than duplicate the efforts of more specialized projects and complicating
item key configuration, connection pooling has not been built directly into
`libzbxpgsql`. Instead we recommend:

 * [PgBouncer](https://pgbouncer.github.io/) or
 * [Pgpool](http://www.pgpool.net/)

Please see the 
[PostgreSQL wiki](https://wiki.postgresql.org/wiki/Replication,_Clustering,_and_Connection_Pooling#Connection_Pooling_and_Acceleration)
for details on pooling.

*NOTE:* If availability monitoring is your primary concern, connection pooling
introduces the potential for false alerts if the pooling daemon fails, while
your PostgreSQL services may still be available. You can mitigate this issue by
using a separate connection string for your availability monitoring keys (e.g.
`pg.connect`) which connects directly to PostgreSQL, bypassing the connection
pooling daemon.