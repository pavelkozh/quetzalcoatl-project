#include <pid.h>
#include <common.h>



void PIDControlInit ( PIDControllerContext_t *ctx )
{
    ctx->err        = 0;
    ctx->prevErr    = 0;
    ctx->integrSum  = 0;
}


controllerRensponse_t PIDControlResponse ( PIDControllerContext_t *ctx )
{
    controllerRensponse_t control = 0;
    if( (ctx->err < ctx->integZone_abs) && (ctx->err > -ctx->integZone_abs))
        ctx->integrSum += ctx->ki * ctx->err;
    else
        ctx->integrSum = 0;
    /* Symmetric limits */
    ctx->integrSum = CLIP_VALUE( ctx->integrSum, -ctx->integrLimit, ctx->integrLimit );
    
    control = ctx->kp * ctx->err +
                ctx->integrSum +
                ctx->kd * (ctx->err - ctx->prevErr);

    ctx->prevErr = ctx->err;
    return control;
}
