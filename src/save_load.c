
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../headers/save_load.h"

/* Save the complete case to a file */
int saveCase(const char *filename, Case *c)
{
    if (filename == NULL || c == NULL ||
        c->people == NULL || c->graph == NULL)
    {
        return 0;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        return 0;
    }

    /* Save people */
    for (int i = 0; i < c->people->size; i++)
    {
        PersonNode *current = c->people->table[i];

        while (current != NULL)
        {
            struct Person *p = &current->person;

            if (fprintf(fp, "PERSONS: %d|%s|%s|%d|%s|%s\n",
                        p->id, p->name, p->role, p->age,
                        p->contact, p->notes) < 0)
            {
                fclose(fp);
                return 0;
            }

            current = current->next;
        }
    }

    /* Save graph relationships */
    for (int i = 0; i < c->graph->maxNodes; i++)
    {
        EdgeNode *edge = getNeighbors(c->graph, i);

        while (edge != NULL)
        {
            if (fprintf(fp, "EDGES: %d|%d|%s|%d|%lld\n",
                        i,
                        edge->destinationId,
                        edge->type,
                        edge->weight,
                        (long long)edge->timespan) < 0)
            {
                fclose(fp);
                return 0;
            }

            edge = edge->next;
        }
    }

    /* Save locations */
    for (int i = 0; i < c->locationCount; i++)
    {
        struct Location *loc = &c->locations[i];

        if (fprintf(fp, "LOCATIONS: %d|%s|%s|%f|%f|%s\n",
                    loc->id, loc->name, loc->type,
                    loc->latitude, loc->longitude,
                    loc->description) < 0)
        {
            fclose(fp);
            return 0;
        }
    }

    /* Save evidence */
    for (int i = 0; i < c->evidenceCount; i++)
    {
        struct Evidence *ev = &c->evidence[i];

        if (fprintf(fp, "EVIDENCE: %d|%s|%s|%d|%d|%lld\n",
                    ev->id, ev->type, ev->description,
                    ev->location_id, ev->person_id,
                    (long long)ev->timespan) < 0)
        {
            fclose(fp);
            return 0;
        }
    }

    /* Save events */
    for (int i = 0; i < c->eventCount; i++)
    {
        struct Event *event = &c->events[i];

        if (fprintf(fp, "EVENTS: %d|%lld|%d|%d|%s|%s\n",
                    event->id,
                    (long long)event->timespan,
                    event->personid,
                    event->locationid,
                    event->type,
                    event->description) < 0)
        {
            fclose(fp);
            return 0;
        }
    }

    if (fclose(fp) != 0)
    {
        return 0;
    }

    return 1;
}

/* Returns the next field of a '|' separated line.
   Empty fields are kept. *cursor becomes NULL after the last field. */
static char *nextField(char **cursor)
{
    if (*cursor == NULL)
        return NULL;

    char *start = *cursor;
    char *bar = strchr(start, '|');

    if (bar != NULL)
    {
        *bar = '\0';
        *cursor = bar + 1;
    }
    else
    {
        *cursor = NULL;
    }
    return start;
}

/* Safely parse an integer field */
static int parseInt(const char *text, int *result)
{
    char *end;
    long value;

    if (text == NULL || *text == '\0')
    {
        return 0;
    }

    value = strtol(text, &end, 10);

    if (*end != '\0' || value < INT_MIN || value > INT_MAX)
    {
        return 0;
    }

    *result = (int)value;
    return 1;
}

/* Safely parse a timestamp */
static int parseTimestamp(const char *text, long long *result)
{
    char *end;
    long long value;

    if (text == NULL || *text == '\0')
    {
        return 0;
    }

    value = strtoll(text, &end, 10);

    if (*end != '\0')
    {
        return 0;
    }

    *result = value;
    return 1;
}

/* Load a saved case from a file */
int loadCase(const char *filename, Case *c)
{
    if (filename == NULL || c == NULL ||
        c->people == NULL || c->graph == NULL)
    {
        return 0;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return 0;
    }

    char line[2048];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        /* Reject lines that exceed the buffer */
        if (strchr(line, '\n') == NULL && !feof(fp))
        {
            fclose(fp);
            return 0;
        }

        line[strcspn(line, "\r\n")] = '\0';

        /* Load people */
        if (strncmp(line, "PERSONS: ", 9) == 0)
        {
            struct Person p = {0};
            char *cursor = line + 9;
            char *field = nextField(&cursor);

            if (!parseInt(field, &p.id))
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL ||
                strlen(field) >= sizeof(p.name))
                goto invalid_record;
            strcpy(p.name, field);

            field = nextField(&cursor);
            if (field == NULL ||
                strlen(field) >= sizeof(p.role))
                goto invalid_record;
            strcpy(p.role, field);

            field = nextField(&cursor);
            if (!parseInt(field, &p.age))
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL ||
                strlen(field) >= sizeof(p.contact))
                goto invalid_record;
            strcpy(p.contact, field);

            field = nextField(&cursor);
            if (field == NULL ||
                strlen(field) >= sizeof(p.notes))
                goto invalid_record;
            strcpy(p.notes, field);

            if (nextField(&cursor) != NULL)
                goto invalid_record;

            if (searchPerson(c->people, p.id) == NULL)
            {
                insertPerson(c->people, p);
                addNode(c->graph, p.id);
            }
        }

        /* Load graph relationships */
        else if (strncmp(line, "EDGES: ", 7) == 0)
        {
            int source, destination, weight;
            long long timestamp;
            char type[100];

            char *cursor = line + 7;
            char *field = nextField(&cursor);
            if (!parseInt(field, &source))
                goto invalid_record;

            field = nextField(&cursor);
            if (!parseInt(field, &destination))
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL || strlen(field) >= sizeof(type))
                goto invalid_record;
            strcpy(type, field);

            field = nextField(&cursor);
            if (!parseInt(field, &weight))
                goto invalid_record;

            field = nextField(&cursor);
            if (!parseTimestamp(field, &timestamp))
                goto invalid_record;

            if (nextField(&cursor) != NULL ||
                source < 0 || destination < 0 ||
                source >= c->graph->maxNodes ||
                destination >= c->graph->maxNodes)
            {
                goto invalid_record;
            }

            addNode(c->graph, source);
            addNode(c->graph, destination);
            addEdge(c->graph, source, destination, type, weight);

            /* Find the newly added edge by its fields */
            EdgeNode *edge = getNeighbors(c->graph, source);

            while (edge != NULL)
            {
                if (edge->destinationId == destination &&
                    edge->weight == weight &&
                    strcmp(edge->type, type) == 0)
                {
                    edge->timespan = (time_t)timestamp;
                    break;
                }

                edge = edge->next;
            }
        }

        /* Load locations */
        else if (strncmp(line, "LOCATIONS: ", 11) == 0)
        {
            struct Location loc = {0};
            char *cursor = line + 11;
            char *field = nextField(&cursor);

            if (!parseInt(field, &loc.id))
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL || strlen(field) >= sizeof(loc.name))
                goto invalid_record;
            strcpy(loc.name, field);

            field = nextField(&cursor);
            if (field == NULL || strlen(field) >= sizeof(loc.type))
                goto invalid_record;
            strcpy(loc.type, field);

            field = nextField(&cursor);
            char *end;
            if (field == NULL)
                goto invalid_record;
            loc.latitude = strtof(field, &end);
            if (*end != '\0')
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL)
                goto invalid_record;
            loc.longitude = strtof(field, &end);
            if (*end != '\0')
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL ||
                strlen(field) >= sizeof(loc.description))
                goto invalid_record;
            strcpy(loc.description, field);

            if (nextField(&cursor) != NULL)
                goto invalid_record;

            if (!addLocation(c, loc))
            {
                int exists = 0;

                for (int i = 0; i < c->locationCount; i++)
                {
                    if (c->locations[i].id == loc.id)
                    {
                        exists = 1;
                        break;
                    }
                }

                if (!exists)
                    goto invalid_record;
            }
        }

        /* Load evidence */
        else if (strncmp(line, "EVIDENCE: ", 10) == 0)
        {
            struct Evidence ev = {0};
            long long timestamp;
            char *cursor = line + 10;
            char *field = nextField(&cursor);

            if (!parseInt(field, &ev.id))
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL || strlen(field) >= sizeof(ev.type))
                goto invalid_record;
            strcpy(ev.type, field);

            field = nextField(&cursor);
            if (field == NULL ||
                strlen(field) >= sizeof(ev.description))
                goto invalid_record;
            strcpy(ev.description, field);

            field = nextField(&cursor);
            if (!parseInt(field, &ev.location_id))
                goto invalid_record;

            field = nextField(&cursor);
            if (!parseInt(field, &ev.person_id))
                goto invalid_record;

            field = nextField(&cursor);
            if (!parseTimestamp(field, &timestamp))
                goto invalid_record;
            ev.timespan = (time_t)timestamp;

            if (nextField(&cursor) != NULL)
                goto invalid_record;

            if (!addEvidence(c, ev))
            {
                int exists = 0;

                for (int i = 0; i < c->evidenceCount; i++)
                {
                    if (c->evidence[i].id == ev.id)
                    {
                        exists = 1;
                        break;
                    }
                }

                if (!exists)
                    goto invalid_record;
            }
        }

        /* Load events */
        else if (strncmp(line, "EVENTS: ", 8) == 0)
        {
            struct Event event = {0};
            long long timestamp;
            char *cursor = line + 8;
            char *field = nextField(&cursor);

            if (!parseInt(field, &event.id))
                goto invalid_record;

            field = nextField(&cursor);
            if (!parseTimestamp(field, &timestamp))
                goto invalid_record;
            event.timespan = (time_t)timestamp;

            field = nextField(&cursor);
            if (!parseInt(field, &event.personid))
                goto invalid_record;

            field = nextField(&cursor);
            if (!parseInt(field, &event.locationid))
                goto invalid_record;

            field = nextField(&cursor);
            if (field == NULL || strlen(field) >= sizeof(event.type))
                goto invalid_record;
            strcpy(event.type, field);

            field = nextField(&cursor);
            if (field == NULL ||
                strlen(field) >= sizeof(event.description))
                goto invalid_record;
            strcpy(event.description, field);

            if (nextField(&cursor) != NULL)
                goto invalid_record;

            if (!addEvent(c, event))
            {
                int exists = 0;

                for (int i = 0; i < c->eventCount; i++)
                {
                    if (c->events[i].id == event.id)
                    {
                        exists = 1;
                        break;
                    }
                }

                if (!exists)
                    goto invalid_record;
            }
        }
    }

    if (ferror(fp))
    {
        fclose(fp);
        return 0;
    }

    if (fclose(fp) != 0)
    {
        return 0;
    }

    return 1;

invalid_record:
    fclose(fp);
    return 0;
}
