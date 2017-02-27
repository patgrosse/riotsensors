/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_rest.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string
assemble_call_success_rest(rs_registered_lambda *lambda, bool cache_retrieved, bool timeout,
                           generic_lambda_return *result) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("success");
    writer.Bool(true);
    writer.Key("lambda");
    {
        writer.StartObject();
        writer.Key("id");
        writer.Uint(lambda->id);
        writer.Key("name");
        writer.String(lambda->name);
        writer.Key("type");
        {
            writer.StartObject();
            writer.Key("code");
            writer.Uint(lambda->type);
            writer.Key("string");
            writer.String(stringify_rs_lambda_type_t(lambda->type));
            writer.EndObject();
        }
        writer.EndObject();
    }
    writer.Key("cache");
    {
        writer.StartObject();
        writer.Key("policy");
        writer.Uint(lambda->cache);
        writer.Key("string");
        writer.String(stringify_rs_cache_type_t(lambda->cache));
        writer.Key("retrieved");
        writer.Bool(cache_retrieved);
        writer.Key("timeout");
        writer.Bool(timeout);
        writer.EndObject();
    }
    writer.Key("result");
    switch (lambda->type) {
        case RS_LAMBDA_INT:
            writer.Int(result->ret_i);
            break;
        case RS_LAMBDA_DOUBLE:
            writer.Double(result->ret_d);
            break;
        case RS_LAMBDA_STRING:
            writer.String(result->ret_s);
            break;
        default:
            break;
    }
    writer.EndObject();
    return s.GetString();
}

std::string assemble_call_error_rest_id(lambda_id_t id, const rs_registered_lambda *lambda, int8_t error) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("success");
    writer.Bool(false);
    writer.Key("lambda");
    {
        writer.StartObject();
        if (lambda != NULL) {
            writer.Key("id");
            writer.Uint(lambda->id);
            writer.Key("name");
            writer.String(lambda->name);
            writer.Key("type");
            {
                writer.StartObject();
                writer.Key("code");
                writer.Uint(lambda->type);
                writer.Key("string");
                writer.String(stringify_rs_lambda_type_t(lambda->type));
                writer.EndObject();
            }
        } else {
            writer.Key("id");
            writer.Uint(id);
            writer.Key("name");
            writer.String("unkown");
            writer.Key("type");
            {
                writer.StartObject();
                writer.Key("code");
                writer.Int(-1);
                writer.Key("string");
                writer.String("none");
                writer.EndObject();
            }
        }
        writer.EndObject();
    }
    writer.Key("error");
    {
        writer.StartObject();
        writer.Key("code");
        writer.Int(error);
        writer.Key("string");
        writer.String(stringify_rs_call_result(error));
        writer.EndObject();
    }
    writer.EndObject();
    return s.GetString();
}

std::string assemble_call_error_rest_name(std::string name, const rs_registered_lambda *lambda, int8_t error) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("success");
    writer.Bool(false);
    writer.Key("lambda");
    writer.StartObject();
    if (lambda != NULL) {
        writer.Key("id");
        writer.Uint(lambda->id);
        writer.Key("name");
        writer.String(lambda->name);
        writer.Key("type");
        writer.StartObject();
        writer.Key("code");
        writer.Uint(lambda->type);
        writer.Key("string");
        writer.String(stringify_rs_lambda_type_t(lambda->type));
        writer.EndObject();
    } else {
        writer.Key("id");
        writer.Int(-1);
        writer.Key("name");
        writer.String(name.c_str());
        writer.Key("type");
        {
            writer.StartObject();
            writer.Key("code");
            writer.Int(-1);
            writer.Key("string");
            writer.String("none");
            writer.EndObject();
        }
    }
    writer.EndObject();
    writer.Key("error");
    {
        writer.StartObject();
        writer.Key("code");
        writer.Int(error);
        writer.Key("string");
        writer.String(stringify_rs_call_result(error));
        writer.EndObject();
    }
    writer.EndObject();
    return s.GetString();
}